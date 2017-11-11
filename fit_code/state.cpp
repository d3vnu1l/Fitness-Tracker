#include "common.h"
#include "utilities.h"
#include "Arduino.h"
#include "EEPROM.h"
#include "eepromUtilities.h"
#include "Display.h"

extern int state, laststate;
extern exercise_strings ex_disps;
extern int nextWorkout;		//holds next workout index, required to set new state after 'warmup'
extern bool canDraw;

void _mainMenu(bool buttonPress, int encoderChange) {
	if (DEBUG_H == true) switchState(benchpress);
	static int index = 0;          //holds menu index 
	int dif = (index + encoderChange);    //get number of encoder turns 
	if (buttonPress == false) {
		if (dif < 0)
			index = 0;		//do not let index variable exceed number of states 
		else if (dif > 2)
			index = 2;
		else
			index = dif;          //map 'mode' variable to states 


			//Display
		if (DEBUG_A == true || DEBUG_H == true);
		else {
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
		}

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
	if (canDraw==true) updateMainMenu(index);
}

void _wod(bool buttonState, int encoderChange) {
	static int nextSelection = 0;
	int index = (nextSelection + encoderChange);
	if (buttonState == false) { 
		if (index < 0)
			nextSelection = 0;		//do not let index variable exceed number of states 
		else if (index >= ex_disps.ex_size)
			nextSelection = ex_disps.ex_size-1;
		else
			nextSelection = index;          //map 'mode' variable to states 

		// /*	//Display
		if (DEBUG_A == false)
			Serial.println(ex_disps.ex_strings[nextSelection]);
		// */
	}
	else {
		//write finalized value
		if (nextSelection == (ex_disps.ex_size - 1))
			switchState(mainMenu);
		else switchState(chooseWeight);
		nextWorkout = nextSelection;
		nextSelection = 0;
	}
	if(canDraw==true) drawScreen(nextSelection, 0);
}

void _chooseWeight(bool buttonState, int encoderChange) {
	static int index = 0;								//index for start / go back selection
	static bool weightIsSet = false;
	int dif;

	//FETCH LAST USED WEIGHT
	static int weight = MAX_ALLOWABLE_WEIGHT + 1;
	if (weight == MAX_ALLOWABLE_WEIGHT + 1) {
		weight = 0;
		if (nextWorkout == 0)
			weight = readInt(WEIGHT_CURLS_ADDR);			//fetch weight from eeprom
		else if (nextWorkout == 1)
			weight = readInt(WEIGHT_BENCHPRESS_ADDR);		//fetch weight from eeprom
		else if (nextWorkout == 2)
			weight = readInt(WEIGHT_SQUATS_ADDR);			//fetch weight from eeprom
	}

	//SELECT WEIGHT
	if (buttonState == false && weightIsSet == false)
	{
		dif = (weight + encoderChange);					//get number of encoder turns
		if (dif >= 0 && dif <= MAX_ALLOWABLE_WEIGHT) {
			weight = dif;
		}
		Serial.print("Weight is ");
		Serial.println(weight);

		//DRAW TFT
		drawScreen(weight, 0);
	}
	else if (weightIsSet == false && buttonState == true) {
		weightIsSet = true;
	}

	//SELECT READY/GOBACK
	else if (weightIsSet == true && buttonState == false)
	{
		dif = (index + encoderChange);					//get number of encoder turns
		if (dif >= 0 && dif <= 1) {
			index = dif;
		}
		if (buttonState == false) {
			switch (index)
			{
			case 0:
				Serial.println("Start");
				break;
			case 1:
				Serial.println("Go Back");
				break;
			}
		}
		//DRAW TFT
		drawScreen(index, 1);
	}
	else {
		switch (index)
		{
		case 0:
			weightIsSet = false;								//reset ready var
			index = 0;											//reset index memory (do not retain cursor position)
			//UPDATE MEMORY EEPROM
			if (nextWorkout == 0)
				writeInt(WEIGHT_CURLS_ADDR, weight);			//update weight from eeprom
			else if (nextWorkout == 1)
				writeInt(WEIGHT_BENCHPRESS_ADDR, weight);			//update weight from eeprom
			else if (nextWorkout == 2)
				writeInt(WEIGHT_SQUATS_ADDR, weight);			//update weight from eeprom
			switchState(warmup);
			break;
		case 1:
			weightIsSet = false;								//reset ready var
			index = 0;											//reset index memory (do not retain cursor position)
			switchState(laststate);
			weight = MAX_ALLOWABLE_WEIGHT + 1;
			break;
		}
	}
	
}

void _warmup(unsigned long &time) {
	if ((time / 1000) < 10) {
		Serial.print("Waiting: ");
		Serial.print(time / 1000);
		Serial.println("/10");
		time = millis();
	}
	else {
		laststate = warmup;
		switch (nextWorkout) {
		case 0: //curls
			nextWorkout = 0;
			switchState(curls);
			break;
		case 1: //benchpress
			nextWorkout = 0;
			switchState(benchpress);
			break;
		case 2: //squats
			nextWorkout = 0;
			switchState(benchpress);
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
		if (elapsed % 1000 == 0) Serial.println(elapsed / 1000);
		elapsed = millis() - start;
		drawScreen(elapsed/1000, 0);
		if ((elapsed / 1000) >= COOLDOWN) {
			counting = false;
			switch (laststate) {
			case curls:
				switchState(mainMenu);
				break;
			case benchpress:
				switchState(mainMenu);
				break;
			case squats:
				switchState(mainMenu);
				break;
			}
		}
	}
}

void _settings(bool buttonPress, int encoderChange) {
	static int index = 0;					//holds menu index
	int dif = (index + encoderChange);		//get number of encoder turns

	if (buttonPress == false) {
		if (dif >= 0 && dif <= 4)			//do not let index variable exceed number of states
			index = dif;					//map 'mode' variable to states

		// /*	//Display
		switch (index)
		{
		case 0:
			Serial.println("Connect\r");
			break;
		case 1:
			Serial.println("Try Again");
			break;
		case 2:
			Serial.println("Yes\r");
			break;
		case 3:
			Serial.println("No\r");
			break;
		case 4:
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
			switchState(laststate);
			break;
		case 2:
			resetMemory();
			switchState(mainMenu);
			//switchState(laststate);
			break;
		case 3:
			switchState(laststate);
			break;
		case 4:
			switchState(laststate);
			break;
		}
		index = 0;
	}
	if (canDraw == true) updateSettings(index);
}

void _personalRecords(bool buttonPress, int encoderChange) {
	static int index = 0;					//holds menu index
	int dif = (index + encoderChange);		//get number of encoder turns
	if (buttonPress == false) {
		if (dif >= 0 && dif < ex_disps.ex_size)			//do not let index variable exceed number of states
			index = dif;					//map 'mode' variable to states


	// /*	//Display
		Serial.println(ex_disps.ex_strings[index]);
		// */
	}
	else {
		switch (index)
		{
		case 0:
			index = 0;
			switchState(laststate);
			break;
		case 1:
			index = 0;
			switchState(laststate);
			break;
		case 2:
			index = 0;
			switchState(laststate);
			break;
		case 3:
			index = 0;
			switchState(laststate);
			break;
		}
	}

	drawScreen(index, 0);
}



