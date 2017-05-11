#include "common.h"
#include "Arduino.h"
extern int state;

const char* stateNames[20] = { "mainMenu", "personalRecords", "wod", "settings", "chooseWeight", "warmup", "curls", "benchpress", "squats", "cooldown" };

void btSend() {
	//String one, two, three;
	//one = stateNames[state];
}
void btReceive() {

}

/*
float f = .222;
int reps = 432;
String thisString = String(reps);
String thisString2 = String(f);
Serial.println(thisString + " " + thisString2);
//String onhe = stateNames[state];
//Serial.println(onhe);
*/