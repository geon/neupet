#ifndef PETINTENTION
#define PETINTENTION

#include "RelativeDirection.h"
#include "Action.h"

struct PetIntention{
	
	PetIntention(Action action, RelativeDirection relativeDirection) : action(action), relativeDirection(relativeDirection) {
	}
	
	Action action;
	RelativeDirection relativeDirection;
};

#endif