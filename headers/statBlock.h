#ifndef STATBLOCK_H_
#define  STATBLOCK_H_

struct statBlock {
	//4 char maximum for strings
	boolean uploaded = false;
	int wrkt;
	int type;
	int reps;
	int weight;   
	float avgTime;   
	float avgEffort;  
	float avgSym;
};

#endif