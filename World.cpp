#include "World.h"
#include "SensoryInput.h"

#include <iostream>
#include <algorithm>
#include <list>


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

	// Generate numPets Pets.
	for(int i=0; i<numPets && i<width*height; ++i){

		if (0 > addPetAndPlaceRandomly(new Pet(), PetState(0, static_cast<Direction>(rand() % numDirections), 1))) {
			
			// Stop if no more pets can fit.
			break;
		}
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

	if (currentState.isAlive()) {

		// Eat plants.
		newState.energy += cells[currentState.position].plantEnergy;
		cells[currentState.position].plantEnergy = 0;
		
		// Clamp the Pet's energy.
		newState.energy = std::min(newState.energy, newState.maxEnergy);
		
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
					addPet(child, PetState(neighbourPosition, direction, breedEnergy / 6));
					break;
				}
			}
			
			newState.energy -= breedEnergy;
		}
		// Does the Pet want to move, and is it possible?
		else if ((petIntention.action & move) && ((!cells[newPosition].pet || !petStates[cells[newPosition].pet].isAlive()) && !cells[newPosition].impassable)) {
			
			// Eat corpses.
			if (cells[newPosition].pet && petStates[cells[newPosition].pet].energy > 0) {
				newState.energy += petStates[cells[newPosition].pet].energy;
				pets.remove(cells[newPosition].pet);
				petStates.erase(cells[newPosition].pet);
			}
			
			// Clamp the Pet's energy.
			newState.energy = std::min(newState.energy, newState.maxEnergy);

			// Actually move.
			newState.direction = newDirection;
			newState.position = newPosition;
			newState.energy -= moveEnergy;
		}

		// Just staying alive takes energy.
		newState.energy -= liveEnergy;
		
		// Handle death.
		if (!newState.isAlive()) {
			
			// Replace the dead Pet with a copy of one random living Pet, to top up the population.
			
			Pet *original = 0;
			
			std::list<Pet*>::iterator candidate = pets.begin();

			// Begin at a random position.
			std::advance(candidate, rand() % pets.size());
			
			// Walk over the list until a living Pet is found, max one lap. 
			for (int i = 0; i < pets.size(); ++i) {
				
				// Make sure the Pet to copy is alive.
				if (petStates[*candidate].isAlive()) {
					
					original = *candidate;
					break;
					
				} else {
					
					// The Pet is dead, so skip to the next.
					++candidate;
					
					// Loop at the end of the list.
					if (candidate == pets.end()) {
						candidate = pets.begin();
					}
				}
			}
			
			// Copy the original.
			if (original) {
				Pet *copy = new Pet(*original);
				addPetAndPlaceRandomly(copy, petStates[original]);
			}
		}

		// Update the stored PetState and WorldCell pointer.
		cells[currentState.position].pet = 0;
		cells[newState.position].pet = pet;
		petStates[pet] = newState;
	}
}


int World::addPetAndPlaceRandomly(Pet *newPet, PetState newState) {

	// Walk over the list until a living Pet is found, max one lap. 
	int position = -1;
	int worldSize = width * height;
	int randomStartPosition = rand() % worldSize;
	for (int i = 0; i < worldSize; ++i) {

		int currentPosition = (randomStartPosition + i) % worldSize;

		// Check if the position is already occupied.
		if (!cells[currentPosition].pet) {

			// If not, take it.
			position = currentPosition;
			break;
		}
	}

	// Place the pet in the position if it is valid.
	if (position >= 0) {
		newState.position = position;
		addPet(newPet, newState);
	}

	return position;
}


bool World::addPet(Pet *newPet, PetState const &newState) {
	
	// refuse to add Pets at positions already occupied.
	if (cells[newState.position].pet) {
		return false;
	}
	
	cells[newState.position].pet = newPet;
	pets.push_front(newPet); // push_front is OK, but not push_back, since the list "pets" is being itterated over while this function is called.
	petStates[newPet] = newState;
	
	return true;
}
