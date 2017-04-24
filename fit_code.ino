#include "common.h"
#include "utilities.h"
#include "dmp.h"
#include "exercises.h"
#include "state.h"
#include "eepromUtilities.h"
#include "EEPROM.h"

// ================================================================
// ===               Global Vars                                ===
// ================================================================
//BUTTONS & MISC VARS//
int buttonState;
int state = start, laststate;
unsigned long time = 0;
//ENCODER VARS//
int encoderPos = 0;
int last_encoderPos = 0;

//MPU VARS//
extern uint16_t packetSize;
extern uint16_t fifoCount;
extern volatile bool mpuInterrupt;

//MPU TRACKING VARS
float buf_YPR[3][BUFFER_SIZE];
int buf_RAWACCEL[3][BUFFER_SIZE];
int buf_WORLDACCEL[3][BUFFER_SIZE];
int buf_smooth_WORLDACCEL[3][BUFFER_SIZE];
unsigned int data_ptr = 0;

// ================================================================
// ===                         MAIN                             ===
// ================================================================
void setup() {
	pinMode(ENCODERPINA, INPUT);
	pinMode(ENCODERPINB, INPUT);
	pinMode(SLED, OUTPUT);
	//EEPROM.write(0, 0);	//force reset
	if (EEPROM.read(INITIALIZED_ADDR) == 0) resetMemory();				//configure memory if first time use
	initBuffers(buf_YPR, buf_WORLDACCEL, buf_smooth_WORLDACCEL);
	dmp_init();
	time = millis();
}

void loop() {
	static bool processedData = false;
	unsigned int last;

	while (!mpuInterrupt && fifoCount < packetSize) {
		readButton(buttonState);
		readEncoder(encoderPos, last_encoderPos);
		if (processedData == false) {
			//EXERCISES GO HERE
			if (state == curls) {   //curls
				_curls(buf_YPR, buf_smooth_WORLDACCEL, data_ptr, state, laststate, buttonState, 200);
			}
			else if (state == benchpress) {                           
				_benchpress(buf_smooth_WORLDACCEL, data_ptr, state, laststate, 40);
			}
			else if (state == squats) {                                                                 
				_squats(buf_smooth_WORLDACCEL, data_ptr, state, laststate, 5);
			}
			processedData = true;
		}

		if (state == start)                                                                   //start
		{
			_start(state, laststate);
		}
		else if (state == chooseExercise)                                                     //chooseexercise
		{
			_chooseExercise(state, laststate, buttonState, encoderPos);
		}
		else if (state == chooseWeight)                                                           //chooseweight
		{
			_chooseWeight(state, laststate, buttonState, encoderPos);
		}
		else if (state == warmup) {                                                                 //warmup
			_warmup(state, laststate, time);
		}
		else if (state == cooldown) {                                                                 //cooldown
			_cooldown(state, laststate);
		}
	}

	//1. handle new data//
	dmp_sample(buf_YPR, buf_WORLDACCEL, buf_RAWACCEL, data_ptr);

	//2. filter new sample//
	if (data_ptr == 0)
		last = (BUFFER_SIZE - 1);
	else last = (data_ptr - 1);
	buf_smooth_WORLDACCEL[2][data_ptr] = iirLPF(buf_WORLDACCEL[2][data_ptr], buf_smooth_WORLDACCEL[2][last], 0.22);
	//buf_smooth_WORLDACCEL[1][data_ptr] = iirLPF(buf_WORLDACCEL[1][data_ptr], 0.21);
	//buf_smooth_WORLDACCEL[0][data_ptr] = iirLPF(buf_WORLDACCEL[0][data_ptr], 0.21);

	//3. flag for new data//
	processedData = false;



}
//NEED A REFERENCE POINT TO RESET VELOCITY & OR HEIGHT
//maybe create a stillness detection or use peakdetect




