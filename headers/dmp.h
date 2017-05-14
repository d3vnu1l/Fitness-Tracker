#ifndef DMP_H_
#define  DMP_H_
#include "common.h"

void dmp_init(void);
bool dmp_sample(int [][BUFFER_SIZE], int [][BUFFER_SIZE], unsigned int &);

#endif  /* DMP_H */


