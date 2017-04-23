#include "common.h"
#include "Arduino.h"
#include "utilities.h"

void _curls(float buf_YPR[][BUFFER_SIZE], int buf_smooth_WORLDACCEL[][BUFFER_SIZE], unsigned int data_ptr, int &state, int &laststate, int buttonState, int reps) {
	static int numreps = -1;
	static float min = 90, max = 0;
	static bool ready = false;
	static bool pivot = false, up = false;
	static float level = 0;
	static int record = 0;
	static int count = 0;
	static float angle[BUFFER_SIZE];

	angle[data_ptr] = abs((abs(buf_YPR[1][data_ptr]) + abs(buf_YPR[2][data_ptr])) - level);

	if (numreps == -1) {

		if (buttonState == LOW && ready == false)
			Serial.println("Level device and press the button to store calibration...");
		else if (buttonState == HIGH && ready == false) {
			level = angle[data_ptr];
			ready = true;

		}
		else if (ready == true) {
			if (angle[data_ptr] <= 30) {
				digitalWrite(SLED, HIGH);    //Turn on the LED
				Serial.print("Hold still: %");
				Serial.println(count);
				count++;
			}
			else {
				digitalWrite(SLED, LOW);    //Turn off the LED
				count = 0;
				Serial.println("move device to level position to begin");
			}
			if (count >= 100) {				//reaching 100 stable cycles begins workout
				numreps = 0;
				count = 0;
				numreps = 0;
				Serial.print("Repetitions: ");
				Serial.println(numreps);
			}
		}
	}
	else if (numreps != -1 && buttonState == LOW) {
		int amag = ((buf_smooth_WORLDACCEL[2][data_ptr]));


		//Serial.print(amag);
		//Serial.print(abs(buf_RAWACCEL[0][data_ptr]));
		//Serial.print(" ");
		//Serial.print(abs(buf_RAWACCEL[1][data_ptr]));
		//Serial.print(" ");
		//Serial.println(abs(buf_RAWACCEL[2][data_ptr]));
		//Serial.print(buf_YPR[0][data_ptr]);
		//Serial.print(", ");
		//Serial.print(buf_YPR[1][data_ptr]);
		//Serial.print(", ");
		//Serial.println(buf_YPR[2][data_ptr]);
		//Serial.print(", ");
		//Serial.print(level);
		//Serial.print(",        ");
		//Serial.println(angle[data_ptr]);
		//Serial.print(", ");
		//Serial.print(pivotDetect(angle, data_ptr, 20));
		//Serial.print(", ");
		//Serial.println(numreps);
		//Serial.print(" dir: ");		


		/*
		Three checks must be completed before a curl is registered:
			1. Angle must be above the CURLS_UP_THRESH
			2. An continuous acceleration must be detected signifying the transition from lifting to lowering
			3. Device must cross back below the minimum threshold angle according to CURLS_DOWN_THRESH
		Some values can be tweaked to adjust the sensitivity & response
			CURLS_UP_THRESH						-max angle that must be surpassed
			CURLS_DOWN_THRESH					-minumum angle after which a curl is registered
			ACC_MAG								-acceleration detection sensitivity
			ACC_MSEC							-number of msecs that acceleration must be present
		*/
		if (angle[data_ptr] > max)
			max = angle[data_ptr];

		if (pivot == false){						//this section looks for the acceleration component caused by switching from up to downward curl
			if (amag < ACC_MAG) {
				count++;
				if (count >= ACC_MSEC && max > CURLS_UP_THRESH) {
					Serial.println("TRIGGERED");
					pivot = true;
					up = true;
					min = 90;
					count = 0;
				}
			}
			else {
				//if (count >=10) Serial.println(count);
				count = 0;
			}
		}
		else if (angle[data_ptr] < CURLS_DOWN_THRESH && up ==true && pivot == true) {				//CHANGE THESE (LOWER REP THRESHOLD)
			if (angle[data_ptr] > min) {
				numreps += 1;
				up = false;
				pivot = false;
				
				Serial.print("Reps: ");
				Serial.print(numreps);
				Serial.print(", Degrs ");
				Serial.println(max);
				max = 0;
				if (numreps > record) {
					record = numreps;
					Serial.println("previous record has been broken!");
				}
			}
		}
		if (angle[data_ptr] < min)
			min = angle[data_ptr];
	}
	else if (numreps != -1 && buttonState == HIGH) {
		numreps = -1;
		state = cooldown;
		laststate = curls;
	}
}

void _benchpress(int buf_smooth_WORLDACCEL[][BUFFER_SIZE], unsigned int data_ptr, int &state, int &laststate, int reps) {
	static int numreps = -1;
	static int velocity[3][BUFFER_SIZE];
	static float vlast = 0;
	static float vnow = 0;
	static float height = 0;
	static float h_max = 0, h_min = 0;
	static int still_zoffset = 0;
	static int dir_last = 0;
	int still, dir;


	if (numreps == -1) {
		numreps = 0;
		Serial.println("benchpress...");
	}
	if (numreps < 5) {
		vnow = (vlast + (0.01 * (buf_smooth_WORLDACCEL[2][data_ptr] - still_zoffset)));
		height = height + (0.01 * vlast) + (0.50 * 0.01 * vnow);
		if (height >= h_max) h_max = height;
		if (height <= h_min) h_min = height;
		vlast = vnow;
		velocity[2][data_ptr] = vnow;

		//Serial.print("reps completed: "); Serial.println(numreps);
		//Serial.print("height: ");
		Serial.print(height);
		Serial.print(", ");
		Serial.print(vnow);
		Serial.print(", ");
		Serial.print(buf_smooth_WORLDACCEL[2][data_ptr]);
		//Serial.println();
		//Serial.print(h_max);
		Serial.print(" reps: ");
		Serial.println(numreps);

		still = detectStill(buf_smooth_WORLDACCEL, data_ptr, still_zoffset, 4, 4);
		//if (velocity[2][data_ptr]> 75 || velocity[2][data_ptr] < -75)
		dir = directionDetect(velocity, data_ptr, 0, 3);
		//		if (dir_last == -200 && dir != -200 && h_max > BENCHPRESS_MAX) {
		if (h_max > BENCHPRESS_MAX && dir_last == -200 && dir != -200) {	//ERROR RESET 
			//h_max = 0;
			//height = 0;
			//vnow = 0;
			//vlast = 0;
			numreps++;
		}
		dir_last = dir;
		//Serial.print(" dir: ");
		//Serial.println(dir);

		if (still == 1) {
			//Serial.println("reset");
			height = 0;
			vlast = 0;
			vnow = 0;
			dir_last = 0;
			dir = 0;
		}
	}
	if (numreps == 5) {
		numreps = -1;
		state = cooldown;
		laststate = benchpress;
	}
}
void _squats(int buf_smooth_WORLDACCEL[][BUFFER_SIZE], unsigned int data_ptr, int &state, int &laststate, int reps) {
	static int numreps = -1;
	static int velocity[3][BUFFER_SIZE];
	static float vlast = 0;
	static float vnow = 0;
	static float height = 0;
	static int still_zoffset = 0;
	int still;

	if (numreps == -1) {
		numreps = 0;
		Serial.println("squats...");
	}
	if (numreps < 5) {
		//still = detectStill(velocity, 15);
		if (still == 1) {
			height = 0;
			vlast = 0;
			vnow = 0;
		}
		vnow = (vlast + (0.01 * (buf_smooth_WORLDACCEL[2][data_ptr] - still_zoffset)));
		height = height + (0.01 * vlast) + (0.50 * 0.01 * vnow);
		vlast = vnow;
		velocity[2][data_ptr] = vnow;
	}
	if (numreps == 5) {
		numreps = -1;
		state = cooldown;
		laststate = squats;
	}
}


