#include "common.h"
#include "Arduino.h"
#include "statBlock.h"
#include "utilities.h"
#include "EEPROM.h"
#include "eepromUtilities.h"
#include "Display.h"

extern int state, laststate;
extern bool canDraw;

void _curls(int buf_YPR[][BUFFER_SIZE], int buf_smooth_WORLDACCEL[][BUFFER_SIZE], unsigned int data_ptr, bool buttonState, int reps) {
	static int numreps = -1;							//number of repetitions
	static int min = 90, max = 0;						//track curl span
	static bool ready = false;
	static bool pivot = false, up = false;
	static int level = 0;
	static int record = 0;
	static int count = 0;
	static int angle[BUFFER_SIZE];
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
				Serial.print("Hold still: %");
				Serial.println(count);
				count++;
			}
			else {
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

		if (pivot == false) {						//this section looks for the acceleration component caused by switching from up to downward curl
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
		else if (angle[data_ptr] < CURLS_DOWN_THRESH && up == true && pivot == true) {				//CHANGE THESE (LOWER REP THRESHOLD)
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
				Serial.println(millis() - timer);
				timer = millis();
				if (numreps > record) {
					record = numreps;
					Serial.println("previous record has been broken!");
				}
			}
		}
		if (angle[data_ptr] < min)
			min = angle[data_ptr];
		if (canDraw == true) drawScreen(numreps, 0);
	}
	else if (numreps == reps) {											//assign next state here
		numreps = -1;
		switchState(cooldown);
	}


}

void _benchpress(int buf_smooth_WORLDACCEL[][BUFFER_SIZE], bool buttonState, unsigned int data_ptr, int reps) {
	static int numreps = -1;	//*				//number of reps, set to -1 before exercise is started
	//height tracking vars
	static int velocity[3][BUFFER_SIZE];
	static int velocity_hpf[3][BUFFER_SIZE];
	static float effort[30];
	static float symmetry[30];
	static int time[30];

	static int vlast = 0;	//*
	static int vnow = 0;	//*
	static float height = 0;	//*
	static float h_max = 0, h_min = 0;	//*
	static float v_max = 0, v_min = 0;
	static float a_max = 0, a_min = 0;
	static float v_median = 0.1;
	static int a_median = 0;
	static int still_zoffset = 0;
	//movement type vars
	static int dir_last = 0;
	int deadstill, vstill, dir;
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
		if (amag < 25)
			vnow = vlast;
		//else calculate new velocity
		else
			vnow = vlast + (.02*buf_smooth_WORLDACCEL[2][data_ptr]);

		//cap velocity, prevent runoff
		if (vnow > VELOCITY_CAP)
			vnow = VELOCITY_CAP;
		else if (vnow < -VELOCITY_CAP)
			vnow = -VELOCITY_CAP;

		velocity[2][data_ptr] = vnow;
		//iirHPFV(velocity, velocity_hpf, data_ptr, 2);
		//velocity_hpf[2][data_ptr] = velocity[2][data_ptr];	//bypass HPF for testing

		//calculate new height
		height = height + (0.02 * velocity[2][data_ptr]);
		dir = directionDetect(velocity, data_ptr, v_median, 18, 1);

		//cap height, prevent runoff
		if (height > HEIGHT_CAP)
			height = HEIGHT_CAP;
		else if (height < -HEIGHT_CAP)
			height = -HEIGHT_CAP;

		//set max/min
		if (height > h_max) h_max = height;
		if (height < h_min) h_min = height;
		if (vnow > v_max) v_max = vnow;
		if (vnow < v_min) v_min = vnow;
		if (buf_smooth_WORLDACCEL[2][data_ptr] > a_max) a_max = buf_smooth_WORLDACCEL[2][data_ptr];
		if (buf_smooth_WORLDACCEL[2][data_ptr] < a_min) a_min = buf_smooth_WORLDACCEL[2][data_ptr];
		//Serial.print("deltV ");
		//Serial.println(v_max - v_min);


		if ((v_max-v_min) > BENCHPRESS_MIN && vlast > v_median && vnow < v_median) {	//ERROR RESET & repcount
			//Serial.print("Height ");
			//Serial.print(h_max-h_min);
			
			int dist = h_max - h_min;

			//if (dist > 200 ) {
				unsigned int time_passed = millis() - timer;
				timer = millis();
				float _effort = (abs(acceleration_accum_up) / (1.0*time_passed));
				v_median = (v_median+((v_max + v_min) / 2))/2.0;
				time[numreps] = time_passed;
				effort[numreps] = _effort;
				symmetry[numreps] = (1.0*acceleration_accum_down / acceleration_accum_up);

				numreps++;

				//DEBUG code
				if (DEBUG_MAT == true);
				else {
					Serial.println(v_median);
					Serial.print("	reps: ");
					Serial.print(numreps);
					Serial.print(", ACCUM up : ");
					Serial.print(acceleration_accum_up);
					Serial.print(", ACCUM dwn : ");
					Serial.print(acceleration_accum_down);
					Serial.print(", time passed: ");
					Serial.print(time[numreps - 1]);
					Serial.print(", Effort: ");
					Serial.print(effort[numreps-1]);
					Serial.print(", symmetry: ");
					Serial.println(symmetry[numreps - 1]);
				}
				height = 0;
				h_max = 0;
				h_min = 0;
				v_min = 0;
				v_max = 0;
				a_min = 0;
				a_max = 0;
				vnow = 0;
				vlast = 0;
				acceleration_accum_down = 0;
				acceleration_accum_up = 0;
			//}
		}

		vlast = vnow;

		//get information about type of movement using stored accelo data
		deadstill = detectStill(buf_smooth_WORLDACCEL, data_ptr, still_zoffset, 15, 6);
		vstill = detectStill(velocity, data_ptr, still_zoffset, 12, 4);
		if (dir == 200) {
			if (buf_smooth_WORLDACCEL[2][data_ptr] > 20)
				acceleration_accum_up += buf_smooth_WORLDACCEL[2][data_ptr];
		}
		if (dir == -200) {
			if (buf_smooth_WORLDACCEL[2][data_ptr] < -20)
				acceleration_accum_down += abs(buf_smooth_WORLDACCEL[2][data_ptr]);
		}

		//slowly reduce estimated height when device is not in motion
		if (vstill == 1) {
			//Serial.println("dec v");
			vnow = 0;
			vlast = 0;
		}

		//reset height tracking when device is on the ground
		if (deadstill == 1) {
			//Serial.print("reset still");
			height = 0;
			vlast = 0;
			vnow = 0;
			dir_last = 0;
			dir = 0;
			acceleration_accum_down = 0;
			acceleration_accum_up = 0;
			v_median = 0.1;
			timer = millis();
			v_max = 0;
			v_min = 0;
			h_max = 0;
			h_min = 0;
			a_min = 0;
			a_max = 0;
		}

		dir_last = dir;

		if (DEBUG_H == true) {
			//for debugging
			//Serial.print(" dir: ");
			Serial.print(dir);
			Serial.print(", ");
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
		}
		if (canDraw == true) drawScreen(numreps, 0);
	}
	if (numreps == reps || buttonState==true) {
		//compute averages
		float avEffort, avSym, avTime;
		for (int i = 0; i < numreps; i++) {
			avEffort += effort[i];
			avTime += time[i];
			avSym += symmetry[i];
		}
		avEffort = avEffort / numreps;
		avTime = (1.0*avTime / numreps) / 1000;
		avSym = avSym / numreps;

		//create stat block and send to memory
		statBlock newSet;
		newSet.uploaded = false;
		newSet.reps = numreps;
		newSet.weight = 133;
		newSet.type = benchpress;
		newSet.wrkt = fiveByFive;
		newSet.avgTime = avTime;
		newSet.avgEffort = avEffort;
		newSet.avgSym = avSym;
		Serial.print("Effort: ");
		Serial.print(avEffort);
		Serial.print(", Time:");
		Serial.print(avTime);
		Serial.print(", Sym:");
		Serial.println(avSym);

		Serial.println("just gonnna send it");
		storeStatBlock(newSet);
		numreps = -1;
		switchState(cooldown);
	}
}
void _squats(int buf_smooth_WORLDACCEL[][BUFFER_SIZE], unsigned int data_ptr, int reps) {
	static int numreps = -1;	//*				//number of reps, set to -1 before exercise is started
								//height tracking vars
	static int velocity[3][BUFFER_SIZE];
	static int velocity_hpf[3][BUFFER_SIZE];
	static float effort[30];
	static float symmetry[30];
	static int time[30];

	static int vlast = 0;	//*
	static int vnow = 0;	//*
	static float height = 0;	//*
	static float h_max = 0, h_min = 0;	//*
										//filter vars
	static float s = .3;
	static float s_alpha = .00005;
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
		Serial.println("squats...");
	}
	if (numreps < reps) {
		//attempt to dampen subtle acceleration effects on velocity
		int amag = abs(buf_smooth_WORLDACCEL[2][data_ptr]);
		if (amag < 250)
			vnow = vlast;

		//else calculate new velocity
		else
			vnow = vlast + (.02*buf_smooth_WORLDACCEL[2][data_ptr]);

		velocity[2][data_ptr] = vnow;

		iirHPFV(velocity, velocity_hpf, data_ptr, 2);

		//calculate new height
		height = height + (0.02 * velocity_hpf[2][data_ptr]);


		if (h_max > BENCHPRESS_MIN && vlast > 0 && vnow < 0) {	//ERROR RESET 
			Serial.print("Height ");
			Serial.print(h_max);

			unsigned int time_passed = millis() - timer;
			timer = millis();
			float _effort = (abs(acceleration_accum_up) / (1.0*time_passed));
			height = 0;
			h_max = 0;

			if (_effort > 1) {
				time[numreps] = time_passed;
				effort[numreps] = _effort;
				symmetry[numreps] = (1.0*acceleration_accum_down / acceleration_accum_up);

				numreps++;
				Serial.print("	reps: ");
				Serial.print(numreps);
				Serial.print(", ACCUM up : ");
				Serial.print(acceleration_accum_up);
				Serial.print(", ACCUM dwn : ");
				Serial.print(acceleration_accum_down);
				Serial.print(", time passed: ");
				Serial.print(time[numreps - 1]);
				//Serial.print(", Effort: ");
				//Serial.println(effort);
				Serial.print(", symmetry: ");
				Serial.println(symmetry[numreps - 1]);
			}
			acceleration_accum_down = 0;
			acceleration_accum_up = 0;
		}

		vlast = vnow;

		//set max/min
		if (abs(height) >= h_max) h_max = abs(height);
		//if (height <= h_min) h_min = height;

		//get information about type of movement using stored accelo data
		deadstill = detectStill(buf_smooth_WORLDACCEL, data_ptr, still_zoffset, 9, 4);
		still = detectStill(buf_smooth_WORLDACCEL, data_ptr, still_zoffset, 3, 10);
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
			height = height*0.999;
		}

		//reset height tracking when device is on the ground
		if (deadstill == 1) {
			//Serial.print("reset still");
			height = 0;
			vlast = 0;
			vnow = 0;
			dir_last = 0;
			dir = 0;
			timer = millis();
		}


		dir_last = dir;

		/*	//for debugging
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
		*/

	}
	if (numreps == reps) {
		//compute averages
		float avEffort, avSym;
		int avTime;
		for (int i = 0; i < numreps; i++) {
			avEffort += effort[i];
			avTime += time[i];
			avSym += symmetry[i];
		}
		Serial.print("Effort: ");
		Serial.print(avEffort / numreps);
		Serial.print(", Time:");
		Serial.print((1.0*avTime / numreps) / 1000);
		Serial.print(", Sym:");
		Serial.println(avSym / numreps);
		numreps = -1;
		switchState(cooldown);
	}
}
void _fitnessTest() {
	//needs implementation
}




