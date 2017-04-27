#include "common.h"
#include "utilities.h"
#include "Arduino.h"
#include "EEPROM.h"
#include "eepromUtilities.h"

int mode = 0;						//used in mapping encoder to exercise selection

void _start(int &state, int &laststate) {
	state = chooseExercise;
	laststate = start;
  //wod
  //settings
}

void _chooseExercise(int &state, int &laststate, bool buttonState, int encoderChange) {
  if (buttonState == false) {
    if (encoderChange == -1) {
      if (mode != 0)
        mode--;
    }
    else if (encoderChange == 1) {
      if (mode != 2)
        mode++;
    }
    switch (mode)
    {
      case 0:
        Serial.println("Curls\r");
        break;
      case 1:
        Serial.println("Bench Press\r");
        break;
      case 2:
        Serial.println("Squats\r");
        break;
    }
  }
  else {
	  //write finalized value
	  switch (mode)
	  {
	  case 0:
		  Serial.println("Curls");
		  break;
	  case 1:
		  Serial.println("Bench Press");
		  break;
	  case 2:
		  Serial.println("Squats");
		  break;
	  }
	  state = chooseWeight;
	  laststate = state;
  }
}
void _chooseWeight(int &state, int &laststate, bool buttonState, int encoderChange) {
  static int weight = readInt(WEIGHT_ADDR);				//fetch weight from eeprom
  if (buttonState == false)
  {
    if (encoderChange == -1) {
      if (weight > 0)
        weight -= 1;
    }
    else if (encoderChange == 1) {
      if (weight < 500)
        weight += 1;
    }
    Serial.print("Weight is ");
    Serial.println(weight);
  }
  else {
	  updateInt(WEIGHT_ADDR, weight);						//update weight in eeprom
	  state = warmup;
	  laststate = chooseWeight;
  }
}
void _warmup(int &state, int &laststate, unsigned long &time) {
  //a timer needds to be started on device powereup. checck for 10 seconds warmup here
  if ((time / 1000) < 4) {
    Serial.print("Waiting: ");
    Serial.print(time / 1000);
    Serial.println("/4");
    time = millis();
  }
  else {
    laststate = warmup;
    switch (mode) {
      case 0: //curls
        state = curls;
        break;
      case 1: //benchpress
        state = benchpress;
        break;
      case 2: //squats
        state = squats;
        break;
    }
  }
}
void _cooldown(int &state, int &laststate) {
  Serial.println("cooldown...");
  for (short int seconds = COOLDOWN; seconds > 0; seconds--) {
    delay(1000);
    Serial.println(seconds);
  }
  switch (laststate) {
    case curls:
      state = curls;
      //state = benchpress;
      break;
    case benchpress:
      state = squats;
      break;
    case squats:
      state = curls;
      break;
  }
}


