#ifndef WORLDCELL
#define WORLDCELL

#include "Pet.h"

struct WorldCell{

	WorldCell();
	
	Pet *pet;
	
	float plantEnergy;
	float plantMaxEnergy;
	float plantGrowth;
	
	bool impassable;
};

#endif