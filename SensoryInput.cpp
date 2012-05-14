#include "SensoryInput.h"
#include "RelativeDirection.h"

#include "World.h"

SensoryInput::SensoryInput(World *world, Pet* pet){
	
	PetState &state = world->petStates[pet];
	
	// Begin sampling at the Pet's position.
	int position = state.position;
	
	// Sample the currentCell
	currentCell = world->cells[position];
	
	// Sample the touchedCell's in all relative directions, in order.
	for(RelativeDirection relativeDirection = firstRelativeDirection; relativeDirection < numRelativeDirections; ++relativeDirection){
		touchedCells[relativeDirection] = world->cells[world->movePosition(position, world->offsetDirectionByRelativeDirection(state.direction, relativeDirection))];
	}
}