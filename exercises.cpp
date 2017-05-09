#include "common.h"
#include "Arduino.h"
#include "utilities.h"
#include "EEPROM.h"

extern int state, laststate;

void _curls(float buf_YPR[][BUFFER_SIZE], int buf_smooth_WORLDACCEL[][BUFFER_SIZE], unsigned int data_ptr, bool buttonState, int reps) {
	static int numreps = -1;							//number of repetitions
	static float min = 90, max = 0;						//track curl span
	static bool ready = false;
	static bool pivot = false, up = false;
	static float level = 0;
	static int record = 0;
	static int count = 0;
	static float angle[BUFFER_SIZE];
	static unsigned long timer;							//time between reps

	angle[data_ptr] = abs((abs(buf_YPR[1][data_ptr]) + abs(buf_YPR[2][data_ptr])) - level);

	if (numreps == -1) {

		if (buttonState == false && ready == false)
			Serial.println("Level device and press the button to store calibration...");
		else if (buttonState == true && ready == false) {
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
				timer = millis();
				Serial.print("Repetitions: ");
				Serial.println(numreps);
			}
		}
	}
	else if (numreps != -1 && numreps < reps && buttonState == false) {
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
				Serial.print(", Degrs: ");
				Serial.print(max);
				max = 0;
				Serial.print(", time: ");
				Serial.println(millis()-timer);
				timer = millis();
				if (numreps > record) {
					record = numreps;
					Serial.println("previous record has been broken!");
				}
			}
		}
		if (angle[data_ptr] < min)
			min = angle[data_ptr];
	}
	else if (numreps != -1 && buttonState == true) {											//assign next state here
		numreps = -1;
		switchState(cooldown);
	}
}

void _benchpress(int buf_smooth_WORLDACCEL[][BUFFER_SIZE], unsigned int data_ptr, int reps) {
	static int numreps = -1;	//*				//number of reps, set to -1 before exercise is started
	//height tracking vars
	static int velocity[3][BUFFER_SIZE];
	static int vlast = 0;	//*
	static int vnow = 0;	//*
	static float height = 0;	//*
	static float h_max = 0, h_min = 0;	//*
	//filter vars
	static float s = .3;
	static float s_alpha = .005;
	static int still_zoffset = 0;
	//movement type vars
	static int dir_last = 0;
	int deadstill, still, dir;
	//stat-track vars
	static long int acceleration_accum_up = 0;		//*
	static long int acceleration_accum_down = 0;	//*
	static long unsigned timer = 0;
	if (timer == 0) {
		timer = millis();
	}

	if (numreps == -1) {
		numreps = 0;
		Serial.println("benchpress...");
	}
	if (numreps < reps) {
		//attempt to dampen subtle acceleration effects on velocity
		int amag = abs(buf_smooth_WORLDACCEL[2][data_ptr]);
		if (amag <= 200)
			vnow = vlast;
		//else calculate new velocity
		else
			vnow = vlast + (.01*buf_smooth_WORLDACCEL[2][data_ptr]);
	

		//HPF velocity
		s = s + (s_alpha*(vnow - s));
		vnow = (vnow - s);

		//calculate new height
		height = height + ((0.01 * vlast) + (0.50 * 0.01 * vnow));

		if (h_max > BENCHPRESS_MAX && vlast == 200 && vnow != 200) {	//ERROR RESET 
			height = 0;
			//vnow = 0;
			//vlast = 0;
			numreps++;
		}






		vlast = vnow;
		velocity[2][data_ptr] = vnow;

		//set max/min
		if (abs(height) >= h_max) h_max = abs(height);
		if (height <= h_min) h_min = height;

		//get information about type of movement using stored accelo data
		deadstill = detectStill(buf_smooth_WORLDACCEL, data_ptr, still_zoffset, 9, 4);
		//still = detectStill(buf_smooth_WORLDACCEL, data_ptr, still_zoffset, 3, 10);
		dir = directionDetect(velocity, data_ptr, 0, 100, 10);

		if (dir == 200) {
			if (buf_smooth_WORLDACCEL[2][data_ptr] > 25)
				acceleration_accum_up += buf_smooth_WORLDACCEL[2][data_ptr];
		}
		if (dir == -200) {
			if (buf_smooth_WORLDACCEL[2][data_ptr] < -25)
				acceleration_accum_down += abs(buf_smooth_WORLDACCEL[2][data_ptr]);
		}

		//slowly reduce estimated height when device is not in motion
		if (still == 1) {
			//Serial.println("dec heihgt");
			//height = height*0.98;
		}

		//reset height tracking when device is on the ground
		if (deadstill == 1) {
			//Serial.print("reset still");
			height = 0;
			vlast = 0;
			vnow = 0;
			dir_last = 0;
			dir = 0;
			timer = 0;
		}

		//count a repetition when specifications are met
		if (h_max > BENCHPRESS_MAX && dir_last == 200 && dir != 200) {	//ERROR RESET 
			//Serial.print("Height ");
			//Serial.print(h_max);
			h_max = 0;
			height = 0;
			//vnow = 0;
			//vlast = 0;
			numreps++;
			unsigned int time_passed = millis() - timer;
			timer = millis();
			float effort = (abs(acceleration_accum_up) / (1.0*time_passed));
			/*
			Serial.print("	reps: ");
			Serial.print(numreps);
			Serial.print(", ACCELERATION ACCUM: ");
			Serial.print(acceleration_accum_up);
			Serial.print(", time passed: ");
			Serial.print(time_passed);
			Serial.print(", Effort: ");
			Serial.println(effort);
			*/

			acceleration_accum_up = 0;
		}
		dir_last = dir;


			//for debugging
		//Serial.print(" dir: ");
		Serial.print(dir);
		Serial.print(" , ");
		//Serial.print("reps completed: "); Serial.println(numreps);
		Serial.print(height);
		Serial.print(", ");
		Serial.print(vnow);
		Serial.print(", ");
		Serial.println(buf_smooth_WORLDACCEL[2][data_ptr]);
		//Serial.print(", ");
		//Serial.println(h_max);
		//Serial.print(" reps: ");
		//Serial.println(numreps);
		//*/
	}
	if (numreps == reps) {
		numreps = -1;
		switchState(cooldown);
	}
}
void _squats(int buf_smooth_WORLDACCEL[][BUFFER_SIZE], unsigned int data_ptr, int reps) {
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
	if (numreps < reps) {
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
	if (numreps == reps) {
		numreps = -1;
		switchState(cooldown);
	}
}
void _fitnessTest() {
	//needs implementation
}

