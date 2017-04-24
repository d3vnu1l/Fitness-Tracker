// eepromUtilities.h

#ifndef _EEPROMUTILITIES_h
#define _EEPROMUTILITIES_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

void writeLong(int, long);
void updateLong(int, long);
long readLong(long);
void writeInt(int, int);
void updateInt(int, int);
int readInt(int);

#endif

