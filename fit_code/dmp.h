#ifndef DMP_H_
#define  DMP_H_
#include "common.h"

void dmp_init(void);
void dmp_sample(int [][BUFFER_SIZE], int [][BUFFER_SIZE], unsigned int &);

void resetF(void);
void enableF(void);

#endif  /* DMP_H */



