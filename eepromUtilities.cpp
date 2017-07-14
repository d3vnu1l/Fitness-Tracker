// Arduino does not provide built in functions for eeprom int and long functions
//		These are built here
#include "Arduino.h"
#include "headers\eepromUtilities.h"
#include "headers\common.h"
#include "EEPROM.h"
#include "headers\statBlock.h"
#include "headers\dmp.h"


void writeInt(int address, int value) {
	byte two = (value & 0xFF);
	byte one = ((value >> 8) & 0xFF);

	EEPROM.write(address, two);
	EEPROM.write(address+1, one);
}

void updateInt(int address, int value) {
	byte two = (value & 0xFF);
	byte one = ((value >> 8) & 0xFF);

	EEPROM.update(address, two);
	EEPROM.update(address + 1, one);
}

int readInt(int address) {
	int two = EEPROM.read(address);
	int one = EEPROM.read(address + 1);

	return ((two << 0) & 0xFF) + ((one << 8) & 0xFFFF);
}

boolean storeStatBlock(statBlock newstat) {
	resetF();
	unsigned int addr = readInt(STAT_POINTER_ADDR);
	Serial.println(addr);
	if (addr == MAXMEM_ADDR) {
		enableF();
		return false;
	}
	else {
		//write 5 ints
		EEPROM.put(addr, newstat);
		addr += sizeof(newstat);
		//write final freemem pointer
		writeInt(STAT_POINTER_ADDR, addr);
		Serial.println(addr);
		enableF();
		return true;
	}
}

void readStatBlock(unsigned int addr) {
	resetF();
	int active_pointer = readInt(STAT_POINTER_ADDR);
	if (addr <= (active_pointer - 11)) {
		//write 5 ints
		statBlock stat;
		EEPROM.get(addr, stat);

		if (stat.uploaded == false)
			Serial.println("Not uploaded");
		else Serial.println("uploaded");
		Serial.print("Weight  ");
		Serial.println(stat.weight);
		Serial.print("reps  ");
		Serial.println(stat.reps);
		Serial.print("wrkt  ");
		Serial.println(stat.wrkt);
		Serial.print("type  ");
		Serial.println(stat.type);
		Serial.print("avgTime  ");
		Serial.println(stat.avgTime);
		Serial.print("avgEff  ");
		Serial.println(stat.avgEffort);
		Serial.print("avgSym  ");
		Serial.println(stat.avgSym);
	}
	enableF();
}

//This function resets the device EEPROM to factory settings
void resetMemory(void) {
	for (int i = 0; i < EEPROM.length(); i++) {
		EEPROM.write(i, 0x00);
	}
	EEPROM.write(INITIALIZED_ADDR, 0xFF);		//set device as configured
	updateInt(WEIGHT_CURLS_ADDR, 150);			//default weight size is 150
	updateInt(WEIGHT_BENCHPRESS_ADDR, 150);		//default weight size is 150
	updateInt(WEIGHT_SQUATS_ADDR, 150);			//default weight size is 150
	writeInt(APP_POINTER_ADDR, STAT_BASEMEM_ADDR);
	writeInt(STAT_POINTER_ADDR, STAT_BASEMEM_ADDR);
}
