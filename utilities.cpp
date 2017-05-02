# include "utilities.h"
# include "common.h"
#include "Wire.h"
#include "Arduino.h"


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


/* Description: places the filtered result from rough array into smooth array using formula: y(i)= alpha*x(i)+(1-alpha)*y(i-1)
	  note; this can be done much quicker using fixed point math.
	  also, because this is an IIR filter, it takes a few samples to stabilize.
   Usage: change alpha to change frequency cutoff.
	  fs=sample rate (100hz), fp=1/fs, RC=fp*((1-alpha)/alpha)
	  cutoff frequency = (1/(2*pi*RC))
*/
void iirLPF(int rough[3][BUFFER_SIZE], int smooth[3][BUFFER_SIZE], unsigned int pointer, int axis, float alpha) {
	unsigned int last;
	if (pointer == 0)
		last = (BUFFER_SIZE - 1);
	else last = (pointer - 1);
	static float alpha_inverse;
	alpha_inverse = 1 - alpha;
	smooth[axis][pointer] = alpha * rough[axis][pointer] + alpha_inverse * smooth[axis][last];
}

//HPF for acceleration, a higher value of alpha yields a higher cutoff frequency.
void iirHPFA(int rough[3][BUFFER_SIZE], int hpf[3][BUFFER_SIZE], unsigned int pointer, int axis, float alpha) {
	static float s = 0;

	s = s + (alpha*(rough[axis][pointer] - s));
	hpf[axis][pointer] = (rough[axis][pointer] - s);
}

//HPF for velocity
//	CURRENTLY UNUSED
float iirHPFV(float sample, float alpha) {
	static float s2 = 0;
	//Serial.println(s2); Serial.print(" , ");
	s2 = s2 + (alpha*(sample - s2));
	//Serial.print(s2); Serial.print(" , ");
	//Serial.println(sample - s2);
	return (sample - s2);
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


