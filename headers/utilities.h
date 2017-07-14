#ifndef UTILITIES_H_
#define  UTILITIES_H_
#include "common.h"

int encoderPressed();
bool buttonPressed();
void initBuffers(int [][BUFFER_SIZE], int [][BUFFER_SIZE], int[][BUFFER_SIZE]);
void printBuffers(void);
void iirLPF(int [3][BUFFER_SIZE], int [3][BUFFER_SIZE], unsigned int, int);
void iirHPFA(int[3][BUFFER_SIZE], int[3][BUFFER_SIZE], unsigned int, int);
void iirHPFV(int[3][BUFFER_SIZE], int[3][BUFFER_SIZE], unsigned int, int);
int pivotDetect(float [BUFFER_SIZE], int, int);
int directionDetect(int[][BUFFER_SIZE], unsigned int, int, int, int);
int detectStill(int [3][BUFFER_SIZE], unsigned int, int, int, int);
void switchState(int);
#endif  /* UTILITIES_H */


