#ifndef STATBLOCK_H_
#define  STATBLOCK_H_

class statBlock {
public:
	//4 char maximum for strings
	int uploaded = -1;
	int wrkt;
	int type;
	int reps;
	int weight;   
	float avgTime;   
	float avgEffort;  
	float avgSym;
};

#endif