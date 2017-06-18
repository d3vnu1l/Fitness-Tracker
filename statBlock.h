#ifndef STATBLOCK_H_
#define  STATBLOCK_H_

class statBlock {
public:
	String wrkt;
	String type;
	int reps;
	double avgTime;   // Length of a box
	double avgEffort;  // Breadth of a box
	int weight;   // Height of a box
};

#endif