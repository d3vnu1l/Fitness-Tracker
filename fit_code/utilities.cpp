# include "utilities.h"
# include "common.h"
#include "Wire.h"
#include "Arduino.h"
extern int state, laststate;
extern int nextWorkout;

//Returns one of 3 values;
// 0  -no movement
// 1  -clockwise
//-1  -counterclockwise
int encoderPressed() {
	static int encALast, encBLast;
	int encAnow = digitalRead(ENCODERPINA);
	int encBnow = digitalRead(ENCODERPINB);

	if (encALast == 0 && encAnow == 1) {
		if (encBnow == 0) {
			encALast = encAnow;
			encBLast = encBnow;
			return 1;
		}
		else {
			encALast = encAnow;
			encBLast = encBnow;
			return -1;
		}
	}

	encALast = encAnow;
	encBLast = encBnow;
	return 0;
}

//returns button press status
//	IMPORTANT: this function returns true only once for each button press
bool buttonPressed() {
	static int lastpress = millis();
	static int buttonState = 0;

	if ((millis() - lastpress) < BTNDELAYMS) 
		return false;
	else {
		int b = digitalRead(BUTTONPIN);
		if (b == 1 && buttonState != b) {
			lastpress = millis();
			buttonState = b;
			return true;
		}
		else {
			buttonState = b;
			return false;
		}
	}
}

//clears buffers to zero. generally only called from the setup() on device startup
void initBuffers(int buf_YPR[][BUFFER_SIZE], int buf_WORLDACCEL[][BUFFER_SIZE], int buf_smooth_WORLDACCEL[][BUFFER_SIZE]) {
	for (int i = 0; i < BUFFER_SIZE; i++) {
		buf_YPR[0][i] = 0;
		buf_YPR[1][i] = 0;
		buf_YPR[2][i] = 0;

		buf_WORLDACCEL[0][i] = 0;
		buf_WORLDACCEL[1][i] = 0;
		buf_WORLDACCEL[2][i] = 0;

		buf_smooth_WORLDACCEL[0][i] = 0;
		buf_smooth_WORLDACCEL[1][i] = 0;
		buf_smooth_WORLDACCEL[2][i] = 0;
	}
}


//lpf for acceleration;			(see: http://www-users.cs.york.ac.uk/~fisher/mkfilter/trad.html)
//5.2hz
void iirLPF(int rough[3][BUFFER_SIZE], int smooth[3][BUFFER_SIZE], unsigned int pointer, int axis) 
{
	static int xv[4] = { .01,.01,.01,.01 };
	static int yv[4] = { .01,.01,.01,.01 };
	const float gain = 13.66142456;

	xv[0] = xv[1]; xv[1] = xv[2];
	xv[2] = rough[axis][pointer] / gain;
	yv[0] = yv[1]; yv[1] = yv[2];
	yv[2] = (xv[0] + xv[2]) + 2 * xv[1]
		+ (-0.3952929601  * yv[0]) + (1.1024977261 * yv[1]);
	smooth[axis][pointer] = yv[2];
	
}

//experimental band pass filter .1-2.5 hz @ 50s/sec
void iirBPF(int rough[3][BUFFER_SIZE], int smooth[3][BUFFER_SIZE], unsigned int pointer, int axis) {
	static int xv[4] = { .01,.01,.01,.01 };
	static int yv[4] = { .01,.01,.01,.01 };
	const float gain = 17.98105591;
	
	xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4];
	xv[4] = rough[axis][pointer] / gain;
	yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4];
	yv[4] = (xv[0] + xv[4]) - 2 * xv[2]
		+ (-0.4584120580 * yv[0]) + (2.1500606825 * yv[1])
		+ (-3.9209736639 * yv[2]) + (3.2292891880 * yv[3]);
	smooth[axis][pointer] = yv[4];
}

//HPF for acceleration, a higher value of alpha yields a higher cutoff frequency.
//1 order	..old 1.008168323
/*
void iirHPFA(int rough[3][BUFFER_SIZE], int hpf[3][BUFFER_SIZE], unsigned int pointer, int axis) 
{
	static int xv[2] = { .8,.8};
	static int yv[2] = { .8,.8};
	const float gain = 1.064177440;
	//Serial.println(yv[0]);
	xv[0] = xv[1];
	xv[1] = rough[axis][pointer]/ gain;
	yv[0] = yv[1];
	yv[1] = (xv[1] - xv[0])
		+ (0.9837957167 * yv[0]);
	hpf[axis][pointer] = yv[1];

}
*/

//HPF for velocity 1hz
void iirHPFV(int rough[3][BUFFER_SIZE], int hpf[3][BUFFER_SIZE], unsigned int pointer, int axis) 
{
	static int xv[2] = { .5,.5 };
	static int yv[2] = { .5,.5 };
	const float gain = 1.003141603;

		;



	xv[0] = xv[1];
	xv[1] = rough[axis][pointer] / gain;
	yv[0] = yv[1];
	yv[1] = (xv[1] - xv[0])
		+ (0.9937364715  * yv[0]);
	hpf[axis][pointer] = yv[1];
}


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



