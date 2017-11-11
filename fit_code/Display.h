#ifndef DISPLAY_H_
#define  DISPLAY_H_
#include "common.h"

void setupScreen(void);
void initScreen(void);
void drawScreen(int, int);

void initCooldown();
void updateCooldown(int);

void initCurls(void);
void initBenchpress(void);
void initSquats(void);

void initWOD(void);
void initMainMenu(void);

void updateExercise(int);
void updateMainMenu(int);
void updateWOD(int);

void initPersonalRecords(void);
void updatePersonalRecords(int);


void drawDeadliftGO(int);
void drawBrGO(int);
void updateSettings(int);
void initSettings(void);

void destroyScreen(void);
void updateBT(void);

void initChooseWeight(void);
void updateChooseWeight(int, int);

#endif