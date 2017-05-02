#include "common.h"
#include "utilities.h"
#include "Arduino.h"
#include "EEPROM.h"
#include "eepromUtilities.h"

extern int state, laststate;
int nextWorkout = 0;		//holds next workout index, required to set new state after 'warmup'

void _mainMenu(bool buttonState, int encoderChange) {
	static int index = 0;					//holds menu index
	int dif = (index + encoderChange);		//get number of encoder turns
	if (buttonState == false) {
		if (dif >= 0 && dif <= 2)			//do not let index variable exceed number of states
			index = dif;					//map 'mode' variable to states


		// /*	//Display
		switch (index)
		{
		case 0:
			Serial.println("W.O.D.\r");
			break;
		case 1:
			Serial.println("P/R\r");
			break;
		case 2:
			Serial.println("Settings\r");
			break;
		}
		// */
	}
	else {
		switch (index)
		{
		case 0:
			switchState(wod);
			break;
		case 1:
			switchState(personalRecords);
			break;
		case 2:
			switchState(settings);
			break;
		}
	}
}

void _wod(bool buttonState, int encoderChange) {
	int dif = (nextWorkout + encoderChange);
	if (buttonState == false) {
		if (dif >= 0 && dif <= 3)
			nextWorkout = dif;					//map 'mode' variable to states

		// /*	//Display
		switch (nextWorkout)
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
		case 3:
			Serial.println("Go Back\r");
			break;
		}
		// */
	}
  else {
	  //write finalized value
	  if (nextWorkout == 3)
		  switchState(laststate);
	  else switchState(chooseWeight);
  }
}

void _chooseWeight(bool buttonState, int encoderChange) {
  static int weight = readInt(WEIGHT_ADDR);				//fetch weight from eeprom
  int dif = (weight + encoderChange);		//get number of encoder turns

  if (buttonState == false)
  {
	  if (dif >= 0 && dif <= 500) {
		  weight = dif;
	  }
    Serial.print("Weight is ");
    Serial.println(weight);
  }
  else {
	  updateInt(WEIGHT_ADDR, weight);						//update weight in eeprom
	  switchState(warmup);
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
    switch (nextWorkout) {
      case 0: //curls
		switchState(curls);
        break;
      case 1: //benchpress
        switchState(benchpress);
        break;
      case 2: //squats
		switchState(squats);
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
				switchState(curls);
				break;
			case benchpress:
				switchState(squats);
				break;
			case squats:
				switchState(curls);
				break;
			}
		}
	}
}

void _settings(bool buttonPress, int encoderChange) {
	static int index = 0;					//holds menu index
	int dif = (index + encoderChange);		//get number of encoder turns
	if (buttonPress == false) {
		if (dif >= 0 && dif <= 2)			//do not let index variable exceed number of states
			index = dif;					//map 'mode' variable to states


											// /*	//Display
		switch (index)
		{
		case 0:
			Serial.println("Bluetooth\r");
			break;
		case 1:
			Serial.println("Clear Profile");
			break;
		case 2:
			Serial.println("Go Back\r");
			break;
		}
		// */
	}
	else {
		switch (index)
		{
		case 0:
			switchState(laststate);
			break;
		case 1:
			resetMemory();
			switchState(mainMenu);
			break;
		case 2:
			switchState(laststate);
			break;
		}
	}
}

void _personalRecords(bool buttonPress, int encoderChange) {

}