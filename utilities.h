#ifndef UTILITIES_H_
#define  UTILITIES_H_
#include "common.h"

int encoderPressed();
bool buttonPressed();
void initBuffers(float [][BUFFER_SIZE], int [][BUFFER_SIZE], int[][BUFFER_SIZE]);
void printBuffers(void);
void iirLPF(int [3][BUFFER_SIZE], int [3][BUFFER_SIZE], unsigned int, int, float);
void iirHPFA(int[3][BUFFER_SIZE], int[3][BUFFER_SIZE], unsigned int, int, float);
float iirHPFV(float, float);
int pivotDetect(float [BUFFER_SIZE], int, int);
int directionDetect(int[][BUFFER_SIZE], unsigned int, int, int, int);
int detectStill(int [][BUFFER_SIZE], unsigned int, int, int, int);
void switchState(int);
#endif  /* UTILITIES_H */


