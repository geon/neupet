#ifndef PETSTATE
#define PETSTATE

#include "Direction.h"

struct PetState{
	
	static const float liveEnergy;
	static const float moveEnergy;
	static const float breedEnergy;
	static const float maxEnergy;
	static const int maxAge;

	PetState();
	PetState(int position, Direction direction, float energy);
	
	Direction direction;
	int position;
	
	float energy;
	int age;

	bool isAlive();
};

#endif