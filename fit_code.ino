#include "headers\common.h"
#include "headers\utilities.h"
#include "headers\dmp.h"
#include "headers\exercises.h"
#include "headers\state.h"
#include "headers\eepromUtilities.h"
#include "headers\blueTooth.h"
#include "headers\Display.h"
#include "EEPROM.h"

// ================================================================
// ===               Global Vars                                ===
// ================================================================
//MISC VARS//
exercise_strings ex_disps;
int state = 0, laststate = 0;
int nextWorkout = 0;
bool bluetoothConnected = true;
unsigned long time = 0;
unsigned long display_tick = 0;
bool canDraw = false;

//MPU VARS//
extern uint16_t packetSize;
extern uint16_t fifoCount;
extern volatile bool mpuInterrupt;

//MPU TRACKING VARS
int buf_YPR[3][BUFFER_SIZE];
int buf_WORLDACCEL[3][BUFFER_SIZE];
int buf_smooth_WORLDACCEL[3][BUFFER_SIZE];
int buf_hpf_WORLDACCEL[3][BUFFER_SIZE];
unsigned int data_ptr = 0;





// ================================================================
// ===                         MAIN                             ===
// ================================================================
void setup() {
  pinMode(ENCODERPINA, INPUT);
  pinMode(ENCODERPINB, INPUT);
  pinMode(BUTTONPIN, INPUT);

  setupScreen();
  if (EEPROM.read(INITIALIZED_ADDR) == 0) resetMemory();				//configures memory if first time use
  initBuffers(buf_YPR, buf_WORLDACCEL, buf_smooth_WORLDACCEL);
  if (UART_ON == true) {
	  Serial.begin(115200); while (!Serial);
  }
  dmp_init();
  time = millis();
  display_tick = millis();

  readStatBlock(534);
}

void loop() {
  static bool processedData = false;
  static bool buttonPress = false;
  static int encoderChange = 0;
  static int state_l = laststate;


  //btSend("test", iTos(state)); //**//

  //************************************************************************************************************
  while (!mpuInterrupt && fifoCount < packetSize) {
    /* IDLE WORK GOES HERE
    		(this section is continuously looping so long as there is no new dmp sample)
    */
    if (buttonPress == true);			//holds button press while system is idling
    else buttonPress = buttonPressed();
    encoderChange += encoderPressed();	//holds encoder var while idling

    /* STATES GO HERE
    		(update rate = 100 Hz flagged after dmp sample)
    */

    if (processedData == false) {
      //kill cycles to emulate 8mhz operation
      //for (int k = 0; k < 10000; k++) _NOP();	//cycle waste to emulate 8mhz

      if (state == mainMenu)                                                                 //start
      {
        _mainMenu(buttonPress, encoderChange);
      }
      else if (state == wod)                                                   //chooseexercise
      {
        _wod(buttonPress, encoderChange);
      }
      else if (state == personalRecords) {
        //drawScreen();
        _personalRecords(buttonPress, encoderChange);
      }
      else if (state == settings) {
        _settings(buttonPress, encoderChange);
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
      else if (state == curls) {   //curls
        _curls(buf_YPR, buf_smooth_WORLDACCEL, data_ptr, buttonPress, 20);
      }
      else if (state == benchpress) {
        _benchpress(buf_smooth_WORLDACCEL, buttonPress, data_ptr, 100);
      }
      else if (state == squats) {
        _squats(buf_smooth_WORLDACCEL, data_ptr, 5);
      }
      processedData = true;
      buttonPress = false;		//temporary workaround
      encoderChange = 0;			//temporary workaround
      if (DEBUG_T == true) Serial.println(micros());
      canDraw = false;
    }
  }
  //************************************************************************************************************
  //Serial.println(micros());
  //1. handle new data//
  //Serial.println(micros());
  if (DEBUG_T == true) Serial.println(micros());
  dmp_sample(buf_YPR, buf_WORLDACCEL, data_ptr);


  //2. filter new sample//
  iirHPFA(buf_WORLDACCEL, buf_hpf_WORLDACCEL, data_ptr, 2);			//High pass filter
  iirLPF(buf_hpf_WORLDACCEL, buf_smooth_WORLDACCEL, data_ptr, 2);		//low pass filter

  //DEBUGGING USE
  if (DEBUG_A == true) {
    Serial.print(buf_WORLDACCEL[2][data_ptr]);
    Serial.print(", ");
    Serial.print(buf_hpf_WORLDACCEL[2][data_ptr]);
    Serial.print(", ");
    Serial.println(buf_smooth_WORLDACCEL[2][data_ptr]);
  }

  //display
  if (state_l != state) {
	resetF();
    initScreen();
	enableF();

  }

  //3. flag that new data is available//
  processedData = false;
  if (canDraw == false) {
    if ((millis() - display_tick) > REFRESH) {
      display_tick = millis();
      canDraw = true;
    }
  }
  state_l = state;
}




