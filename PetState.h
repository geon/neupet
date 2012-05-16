#ifndef PETSTATE
#define PETSTATE

#include "Direction.h"

struct PetState{
	
	PetState();
	PetState(int position, Direction direction, float energy);
	
	Direction direction;
	int position;
	
	float energy;
	float maxEnergy;
	
	bool isAlive();
};

#endif