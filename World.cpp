#include "World.h"
#include "SensoryInput.h"

#include <iostream>
#include <algorithm>


const float World::liveEnergy = 1/2000;
const float World::moveEnergy = 1/1000;
const float World::breedEnergy = 0.5;

World::World(){
	
	buildCage(6);
}


World::~World(){
	
	for(std::list<Pet *>::iterator i=pets.begin(); i != pets.end(); ++i){
		delete *i;
	}
	
}


int World::coordinateToIndex(int x, int y){
	return y*width + x;
}


int World::movePosition(int position, Direction direction){
	
	int x = position % width;
	int y = position / width;
	
	// Odd rows are indented half a hexagon.
	switch (direction) {
			
		case left:
			--x;
			break;
		case downLeft:
			if (! (y % 2))
				--x;
			++y;
			break;
		case upLeft:
			if (! (y % 2))
				--x;
			--y;
			break;
		case right:
			++x;
			break;
		case upRight:
			if (y % 2)
				++x;
			--y;
			break;
		case downRight:
			if (y % 2)
				++x;
			++y;
			break;
			
		default:
			break;
	}
	
	x = (x + width) % width;
	y = (y + height) % height;
	
	return coordinateToIndex(x, y);
}


Direction World::offsetDirectionByRelativeDirection(Direction direction, RelativeDirection relativeDirection){
	
	return static_cast<Direction>(
		(static_cast<int>(direction) + static_cast<int>(numDirections) + static_cast<int>(relativeDirection)) % static_cast<int>(numDirections)
	);
}


void World::buildCage(int sideLength){
	// Draw a "cage" of walls with one side open.
	
	// Begin in the center.
	int position = coordinateToIndex(width/2, height/2);
	
	// Move to starting position
	for(int i=0; i<sideLength; ++i){
		position = movePosition(position, upLeft);
	}
	cells[position].impassable = true;
	
	// Draw a semi circle. (5 sides of 6.)
	Direction direction = downLeft;
	for(int j=0; j<5; ++j){
		
		// Draw one side.
		for(int i=0; i<sideLength; ++i){
			
			// Take a step.
			position = movePosition(position, direction);
			
			// Place wall.
			cells[position].impassable = true;
		}
		
		// Turn.
		direction = offsetDirectionByRelativeDirection(direction, forwardLeft);
	}
}


void World::generatePopulation(int numPets){
	for(int i=0; i<numPets && i<width*height; ++i){
		Pet *pet = new Pet();
		int position = i * (width*height)/(float)numPets;
		cells[position].pet = pet;
		pets.push_back(pet);
		petStates[pet] = PetState(position, static_cast<Direction>(rand() % numDirections), 1);
	}
}


void World::sprinklePlants(int numPlants){
	for(int i=0; i<numPlants; ++i){
		int position = rand() % (width*height);
		WorldCell &cell = cells[position];
		
		cell.plantGrowth = 0.005;
		cell.plantEnergy = cell.plantMaxEnergy * rand() / (float) RAND_MAX;
	}
}


void World::render(){
	
	for(int y=0; y < height; ++y){
		
		// Begin a new line.
		std::cout << std::endl;
		
		// Indent odd lines to line up hexagonally.
		if(y % 2)
			std::cout << ' ';
		
		for(int x=0; x < width; ++x){
			
			// Get the relevant cell.
			WorldCell &cell = cells[coordinateToIndex(x, y)];
			
			// Mark any occupied cell.
			std::cout << (cell.pet ? (petStates[cell.pet].energy > 0 ? '*' : 'o') : (cell.impassable ? 'X' : (cell.plantEnergy > 0 ? (cell.plantEnergy > 0.5 ? 'A' : '^') : ' ')));
			
			// Spacing for hexagonal layout.
			std::cout << ' ';
		}
	}
	
}


void World::step(){
	
	// Update Pets.
	for(std::list<Pet *>::iterator i=pets.begin(); i != pets.end(); ++i){
		Pet *pet = *i;
		applyPetIntentionToPet(pet, pet->getPetIntentionForSensoryInput(SensoryInput(this, pet)));
	}
	
	// Update WorldCells.
	for (int i=0; i<width*height; ++i) {
		WorldCell &cell = cells[i];
		
		// Plant growth.
		cell.plantEnergy = std::min(cell.plantEnergy + cell.plantGrowth, cell.plantMaxEnergy);
	}
}


void World::applyPetIntentionToPet(Pet *pet, PetIntention petIntention){
	
	PetState &currentState = petStates[pet];
	PetState newState = currentState;

	// Just staying alive takes energy.
	newState.energy -= liveEnergy;

	// Eat plants.
	newState.energy += cells[currentState.position].plantEnergy;
	cells[currentState.position].plantEnergy = 0;

	if (currentState.energy > 0) {
		Direction oldDirection = currentState.direction;
		Direction newDirection = offsetDirectionByRelativeDirection(oldDirection, petIntention.relativeDirection);
		
		// Calculate the target position.
		int newPosition = movePosition(currentState.position, newDirection);
		
		// Does the Pet want to mate, and is it possible?
		if ((petIntention.action & mate) && cells[newPosition].pet && currentState.energy > breedEnergy) {
			
			// Mate
			for (Direction direction = firstDirection; direction < numDirections; ++direction) {
				int neighbourPosition = movePosition(currentState.position, direction);
				WorldCell &cell = cells[neighbourPosition];
				if (!cell.pet && !cell.impassable) {
					Pet *child = new Pet(*pet, *(cells[newPosition].pet));
					cell.pet = child;
					pets.push_front(child); // push_front is OK, but not push_back, since the list is being itterated over while this function is called.
					petStates[child] = PetState(neighbourPosition, direction, breedEnergy / 5);
					break;
				}
			}
			cells[currentState.position].pet = NULL;
			cells[newPosition].pet = pet;
			
			newState.energy -= breedEnergy;
		}
		// Does the Pet want to move, and is it possible?
		else if ((petIntention.action & move) && ((!cells[newPosition].pet || petStates[cells[newPosition].pet].energy < 0.001) && !cells[newPosition].impassable)) {
			
			// Eat corpses.
			if (cells[newPosition].pet && petStates[cells[newPosition].pet].energy > 0) {
				newState.energy += petStates[cells[newPosition].pet].energy;
				pets.remove(cells[newPosition].pet);
				petStates.erase(cells[newPosition].pet);
			}
			
			// Actually move.
			newState.direction = newDirection;
			newState.position = newPosition;
			newState.energy -= moveEnergy;
		}
	}

	// Clamp the Pet's energy.
	newState.energy = std::min(newState.energy, newState.maxEnergy);
	
	// Update the stored PetState and WorldCell pointer.
	cells[currentState.position].pet = NULL;
	cells[newState.position].pet = pet;
	petStates[pet] = newState;
}