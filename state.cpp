#include "common.h"
#include "utilities.h"
#include "Arduino.h"
#include "EEPROM.h"

int mode = 0;

void _start(int &state, int &laststate) {
  Serial.println("Rotate upper dial to enter an exercise");
  //Serial.println(A0);
  //Serial.println(A1);
  state = chooseExercise;
  laststate = start;
}
void _chooseExercise(int &state, int &laststate, int buttonState, int encoderPos) {
  static int last_encoderPos = 0;
  static bool triggered = false;
  if (buttonState == LOW && triggered == false) {
    if (last_encoderPos > encoderPos) {
      if (mode != 0)
        mode--;
    }
    else if (last_encoderPos < encoderPos) {
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
    last_encoderPos = encoderPos;
  }
  else if (buttonState==HIGH){
	  triggered = true;
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
void _chooseWeight(int &state, int &laststate, int &buttonState, int &encoderPos) {
  static int weight = 150;
  static int last_encoderPos = 0;
  static bool triggered = false;
  if (buttonState == LOW && triggered==false)
  {
    if (last_encoderPos > encoderPos) {
      if (weight > 0)
        weight -= 10;
    }
    else if (last_encoderPos < encoderPos) {
      if (weight < 500)
        weight += 10;
    }
    Serial.print("Weight is ");
    Serial.println(weight);
    last_encoderPos = encoderPos;
  }
  else if (buttonState==HIGH){
	  triggered = true;
  }
  else {
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


