#include "common.h"
#include "Arduino.h"
extern int state;

const char* stateNames[20] = { "mainMenu", "personalRecords", "wod", "settings", "chooseWeight", "warmup", "curls", "benchpress", "squats", "cooldown" };

void btSend(String two, String three) {
	String outgoing;
	String one = stateNames[state];

	outgoing = one + "~" + two + "~" + three + "#";
	Serial.print(outgoing);
	Serial.print(":  ");


}

void btSend(String two) {
	String outgoing;
	String one = stateNames[state];

	outgoing = one + "~" + two + "#";
	Serial.print(outgoing);
	Serial.print(":  ");


}

void btSendSys(String one, String two, String three) {
	String outgoing = one + "~" + two + "~" + three + "#";
}

void btReceive() {
	//not implemented yet
}

String iTos(int convertMe) {
	return String(convertMe);
}


/* Conversion example
float f = .222;
int reps = 432;
String thisString = String(reps);
String thisString2 = String(f);
Serial.println(thisString + " " + thisString2);
//String onhe = stateNames[state];
//Serial.println(onhe);
*/