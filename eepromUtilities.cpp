// Arduino does not provide built in functions for eeprom int and long functions
//		These are built here
#include "Arduino.h"
#include "eepromUtilities.h"
#include "common.h"
#include "EEPROM.h"

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
