// eepromUtilities.h

#ifndef _EEPROMUTILITIES_h
#define _EEPROMUTILITIES_h

#include "statBlock.h"
#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

void writeInt(int, int);
void updateInt(int, int);
int readInt(int);
void resetMemory(void);
boolean storeStatBlock(statBlock);
void readStatBlock(unsigned int);

#endif

