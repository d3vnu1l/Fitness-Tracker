// Arduino does not provide built in functions for eeprom int and long functions
//		These are built here
#include "Arduino.h"
#include "headers\eepromUtilities.h"
#include "headers\common.h"
#include "EEPROM.h"
#include "headers\statBlock.h"
#include "headers\dmp.h"

void writeLong(int address, long value) {
	//Decomposition from a long to 4 bytes by using bitshift.
	//One = Most significant -> Four = Least significant byte
	byte four = (value & 0xFF);
	byte three = ((value >> 8) & 0xFF);
	byte two = ((value >> 16) & 0xFF);
	byte one = ((value >> 24) & 0xFF);

	//Write the 4 bytes into the eeprom memory.
	EEPROM.write(address, four);
	EEPROM.write(address + 1, three);
	EEPROM.write(address + 2, two);
	EEPROM.write(address + 3, one);
}

void updateLong(int address, long value) {
	//Decomposition from a long to 4 bytes by using bitshift.
	//One = Most significant -> Four = Least significant byte
	byte four = (value & 0xFF);
	byte three = ((value >> 8) & 0xFF);
	byte two = ((value >> 16) & 0xFF);
	byte one = ((value >> 24) & 0xFF);

	//Write the 4 bytes into the eeprom memory.
	EEPROM.update(address, four);
	EEPROM.update(address + 1, three);
	EEPROM.update(address + 2, two);
	EEPROM.update(address + 3, one);
}

long readLong(long address){
	//Read the 4 bytes from the eeprom memory.
	long four = EEPROM.read(address);
	long three = EEPROM.read(address + 1);
	long two = EEPROM.read(address + 2);
	long one = EEPROM.read(address + 3);

	//Return the recomposed long by using bitshift.
	return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

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
		writeInt(addr, newstat.uploaded);
		addr = addr + 2;
		writeInt(addr, newstat.wrkt);
		addr = addr + 2;
		writeInt(addr, newstat.type);
		addr = addr + 2;
		writeInt(addr, newstat.reps);
		addr = addr + 2;
		writeInt(addr, newstat.weight);
		addr = addr + 2;
		//write 3 longs
		writeLong(addr, newstat.avgTime);
		addr = addr + 4;
		writeLong(addr, newstat.avgEffort);
		addr = addr + 4;
		writeLong(addr, newstat.avgSym);
		addr = addr + 4;
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
	if (addr <= (active_pointer - 12)) {
		//write 5 ints
		statBlock stat;
		stat.uploaded = (addr);
		addr = addr + 2;
		stat.wrkt = readInt(addr);
		addr = addr + 2;
		stat.type = readInt(addr);
		addr = addr + 2;
		stat.reps = readInt(addr);
		addr = addr + 2;
		stat.weight = readInt(addr);
		addr = addr + 2;
		//write 3 longs
		stat.avgTime = readLong(addr);
		addr = addr + 4;
		stat.avgEffort = readLong(addr);
		addr = addr + 4;
		stat.avgSym = readLong(addr);

		if (stat.uploaded == -1)
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
	EEPROM.write(INITIALIZED_ADDR, 0xFF);		//set device as configured
	updateInt(WEIGHT_CURLS_ADDR, 150);				//default weight size is 150
	updateInt(WEIGHT_BENCHPRESS_ADDR, 150);				//default weight size is 150
	updateInt(WEIGHT_SQUATS_ADDR, 150);				//default weight size is 150
	writeInt(APP_POINTER_ADDR, STAT_BASEMEM_ADDR);
	writeInt(STAT_POINTER_ADDR, STAT_BASEMEM_ADDR);
	for (int i = STAT_BASEMEM_ADDR; i < 4095; i += 2) {
		EEPROM.write(i, 0x00);
	}
}
