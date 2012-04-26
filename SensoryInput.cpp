#include "SensoryInput.h"
#include "RelativeDirection.h"

#include "World.h"

SensoryInput::SensoryInput(World *world, Pet* pet){
	
	// Begin sampling at the Pet's position.
	int position = world->petPositions[pet];
	
	// Sample the currentCell
	currentCell = world->cells[position];
	
	// Sample the touchedCell's in all relative directions, in order.
	for(int relativeDirection=0; relativeDirection<numRelativeDirections; ++relativeDirection){
		touchedCells[relativeDirection] = world->cells[world->movePosition(position, world->offsetDirectionByRelativeDirection(world->petDirections[pet], static_cast<RelativeDirection>(relativeDirection)))];
	}
}