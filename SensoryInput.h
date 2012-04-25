#ifndef SENSORYINPUT
#define SENSORYINPUT

#include "Direction.h"

class World;
class Pet;
#include "WorldCell.h"

// This should contain everything the Pet can sense.
struct SensoryInput{
	
	SensoryInput(World *world, Pet* pet);
	
	// TODO: Change all members to constants, and the WorldCells to references.
	
	WorldCell currentCell;
	WorldCell touchedCells[numDirections];
};

#endif