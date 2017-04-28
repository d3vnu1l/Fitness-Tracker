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
//MISC VARS//
int state = 0, laststate = 0;
unsigned long time = 0;

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
	static bool processedState = false;
	static bool buttonPress = false;
	static bool encoderChange = false;

	//************************************************************************************************************
	while (!mpuInterrupt && fifoCount < packetSize) {
		/* IDLE WORK GOES HERE 
				(this section is continuously looping so long as there is no new dmp sample)
		*/
		if (buttonPress == true);			//holds button press while system is idling
		else buttonPress = buttonPressed();
		encoderChange = encoderPressed();	//encoder state var


		/* STATES RECEIVE GO HERE
				(update rate = 100 Hz flagged after dmp sample)
		*/
		if (processedData == false) {
			if (state == mainMenu)                                                                 //start
			{
				_mainMenu();
			}
			else if (state == wod)                                                   //chooseexercise
			{
				_wod(buttonPress, encoderChange);
			}
			else if (state == chooseWeight)                                                     //chooseweight
			{
				_chooseWeight(buttonPress, encoderChange);
			}
			else if (state == warmup) {                                                          //warmup
				_warmup(time);
			}
			else if (state == cooldown) {                                                        //cooldown
				_cooldown();
			}
			processedData = true;
			buttonPress = false;		//reset button (temporary workaround)
		}
		/* OTHER STATES
				(update rate = 100 Hz flagged after dmp sample) 
		*/
		(100 Hz)
		if (processedState == false) {
			if (state == curls) {   //curls
				_curls(buf_YPR, buf_smooth_WORLDACCEL, data_ptr, buttonPress, 200);
			}
			else if (state == benchpress) {
				_benchpress(buf_smooth_WORLDACCEL, data_ptr, 200);
			}
			else if (state == squats) {
				_squats(buf_smooth_WORLDACCEL, data_ptr, 200);
			}
			processedState = true;
			buttonPress = false;		//temporary workaround
		}
	}
	//************************************************************************************************************
	
	//1. handle new data//
	dmp_sample(buf_YPR, buf_WORLDACCEL, buf_RAWACCEL, data_ptr);

	//2. filter new sample//
	iirLPF(buf_WORLDACCEL, buf_smooth_WORLDACCEL, data_ptr, 2, 0.22);  

	//3. flag that new data is available//
	processedData = false;
	processedState = false;

}




