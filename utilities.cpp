# include "utilities.h"
# include "common.h"
#include "Wire.h"
#include "Arduino.h"
extern int state, laststate;

//Returns one of 3 values;
// 0  -no movement
// 1  -clockwise
//-1  -counterclockwise
int encoderPressed() {
	static int encoder0PinALast = LOW;

	int n;
	n = digitalRead(ENCODERPINA);
	if ((encoder0PinALast == LOW) && (n == HIGH)) {
		if (digitalRead(ENCODERPINB) == LOW) {
			encoder0PinALast = n;
			//Serial.println("UP");
			_delay_ms(5);
			//delayMicroseconds(305); //debounce
			return 1;
		}
		else {
			encoder0PinALast = n;
			//Serial.println("down");
			_delay_ms(5);
			//delayMicroseconds(305); //debounce
			return -1;
		}
	} 
	else {
		encoder0PinALast = n;
		return 0;
	}
}

//returns button press status
//	IMPORTANT: this function returns true only once for each button press
bool buttonPressed() {
	static int buttonState = 0;
	int b = digitalRead(BUTTONPIN);
	if (b==1 && buttonState != b) {
		buttonState = b;
		_delay_ms(5);
		return true;
	}
	else {
		buttonState = b;
		return false;
	}
}

//clears buffers to zero. generally only called from the setup() on device startup
void initBuffers(float buf_YPR[][BUFFER_SIZE], int buf_WORLDACCEL[][BUFFER_SIZE], int buf_smooth_WORLDACCEL[][BUFFER_SIZE]) {
	for (int i = 0; i < BUFFER_SIZE; i++) {
		buf_YPR[0][i] = 0.0;
		buf_YPR[1][i] = 0.0;
		buf_YPR[2][i] = 0.0;

		buf_WORLDACCEL[0][i] = 0;
		buf_WORLDACCEL[1][i] = 0;
		buf_WORLDACCEL[2][i] = 0;

		buf_smooth_WORLDACCEL[0][i] = 0;
		buf_smooth_WORLDACCEL[1][i] = 0;
		buf_smooth_WORLDACCEL[2][i] = 0;
	}
}


//lpf for acceleration;			(see: http://www-users.cs.york.ac.uk/~fisher/mkfilter/trad.html)
void iirLPF(int rough[3][BUFFER_SIZE], int smooth[3][BUFFER_SIZE], unsigned int pointer, int axis) {
	static float xa[2], ya[2];
	static float gain = 16.89454484;

	xa[0] = xa[1];
	xa[1] = rough[axis][pointer] / gain;
	ya[0] = ya[1];
	ya[1] = (xa[1] + xa[0]) + (0.8816185924 * ya[0]);
	smooth[axis][pointer] = ya[1];
}

//HPF for acceleration, a higher value of alpha yields a higher cutoff frequency.
void iirHPFA(int rough[3][BUFFER_SIZE], int hpf[3][BUFFER_SIZE], unsigned int pointer, int axis) {
	static float xa[2], ya[2];
	static float gain = 1.003141603;

	xa[0] = xa[1];
	xa[1] = rough[axis][pointer] / gain;
	ya[0] = ya[1];
	ya[1] = (xa[1] - xa[0]) + (0.9937364715 * ya[0]);
	hpf[axis][pointer] = ya[1];

}

//HPF for velocity
float iirHPFV(int rough[3][BUFFER_SIZE], int hpf[3][BUFFER_SIZE], unsigned int pointer, int axis) {
	static float xv[2], yv[2];
	static float gain = 1.004712424;

	xv[0] = xv[1];
	xv[1] = rough[axis][pointer] / gain;
	yv[0] = yv[1];
	yv[1] = (xv[1] - xv[0]) + (0.9906193578 * yv[0]);
	hpf[axis][pointer] = yv[1];
}

/*
int pivotDetect(float harray[BUFFER_SIZE], int array_ptr, int size) {
	int signs = 0;
	int depth = size / 2;
	int midpoint = array_ptr - depth;
	if (depth < 0)
		midpoint = array_ptr + depth;

	int rightseek;
	int leftseek = midpoint;
	if (leftseek == (BUFFER_SIZE-1))
		int rightseek = 0;
	else int rightseek = (leftseek + 1);

	for (int j = 0; j < depth; j++) {
		if (abs(harray[leftseek] - harray[rightseek]) <= 0.5) signs -= 1;
		else signs += (abs(harray[leftseek]) - abs(harray[rightseek]));
		if (leftseek == 0)
			leftseek = (BUFFER_SIZE - 1);
		else leftseek = leftseek - 1;
		if (rightseek == (BUFFER_SIZE-1))
			rightseek = 0;
		else rightseek = rightseek + 1;
	}

	return signs;
}
*/


//Find direction of data trend. Returns
//1.	0 if not moving
//2.	200 if increasing in + dir
//3.	-200 if decreasing in - dir
int directionDetect(int varray[3][BUFFER_SIZE], unsigned int array_ptr, int still_zoffset, int sensitivity, int samples) {
	float avg = 0;
	int last_last;
	int last = array_ptr;
	if (last == 0)
		int last_last = (BUFFER_SIZE - 1);
	else int last_last = (last - 1);
	if (varray[2][last] <= (-sensitivity + still_zoffset) || varray[2][last] >= (sensitivity + still_zoffset)) {
		for (int j = 0; j < samples; j++) {
			if (last - 1 < 0)
				last = (BUFFER_SIZE - 1);
			else last = last - 1;
			if (last_last - 1 < 0)
				last_last = (BUFFER_SIZE - 1);
			else last_last = last_last - 1;
			avg += varray[2][last];
		}
	}
	if (avg == 0)
		return 0;
	avg = avg / samples;
	if (avg > still_zoffset)
		return 200;
	else if (avg < -still_zoffset)
		return -200;
	else if (avg == 0)
		return -1;
	else return 0;
}

//used to detect how still the device is.
int detectStill(int garray[3][BUFFER_SIZE], unsigned int array_ptr, int still_zoffset, int still_size, int tolerance) {
	int last = array_ptr;
	int match = garray[2][last];
	int match_h = match + tolerance;
	int match_l = match - tolerance;
	for (int j = 0; j < still_size; j++) {
		if (last - 1 < 0)
			last = (BUFFER_SIZE - 1);
		else last = last - 1;
		if (garray[2][last] <= match_h && garray[2][last] >= match_l);
		else return 0;
	}
	still_zoffset = match;
	return 1;
}

void switchState(int newstate) {
	laststate = state;
	state = newstate;
}

