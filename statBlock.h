#ifndef STATBLOCK_H_
#define  STATBLOCK_H_

class statBlock {
public:
	//4 char maximum for strings
	int wrkt;
	int type;
	boolean uploaded = false;
	int reps;
	float avgTime;   // Length of a box
	float avgEffort;  // Breadth of a box
	float sym;
	int weight;   // Height of a box
};

#endif