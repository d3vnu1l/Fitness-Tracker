#include "common.h"
#include "utilities.h"
#include "Arduino.h"
#include "EEPROM.h"
#include "eepromUtilities.h"

extern int state, laststate;
static int mode = 0;

void _mainMenu() {
	state = wod;
	laststate = mainMenu;
	//wod
	//settings
	//stats
	//fitness test tbd
}

void _wod(bool buttonState, int encoderChange) {
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
void _chooseWeight(bool buttonState, int encoderChange) {
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

void _warmup(unsigned long &time) {
  if ((time / 1000) < 3) {
    Serial.print("Waiting: ");
    Serial.print(time / 1000);
    Serial.println("/3");
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
void _cooldown() {
	static unsigned long start, elapsed;
	static bool counting = false;
	if (counting == false) {
		Serial.println("cooldown...");
		start = millis();
		counting = true;
	}
	if (counting == true) {
		Serial.println(elapsed / 1000);
		elapsed = millis() - start;
		if ((elapsed / 1000) >= COOLDOWN) {
			counting = false;
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
	}
}

void _settings() {

}