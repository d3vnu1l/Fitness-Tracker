#ifndef UTILITIES_H_
#define  UTILITIES_H_
#include "common.h"

int encoderPressed();
bool buttonPressed();
void initBuffers(float [][BUFFER_SIZE], int [][BUFFER_SIZE], int[][BUFFER_SIZE]);
void printBuffers(void);
int iirLPF(int, int, float);
int pivotDetect(float [BUFFER_SIZE], int, int);
int directionDetect(int[][BUFFER_SIZE], unsigned int, int, int);
int detectStill(int [][BUFFER_SIZE], unsigned int, int &, int, int);
#endif  /* UTILITIES_H */


