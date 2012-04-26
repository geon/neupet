#include "World.h"
#include "SensoryInput.h"

#include <iostream>
#include <algorithm>


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
		petPositions[pet] = position;
		petDirections[pet] = static_cast<Direction>(rand() % numDirections);
	}
}


void World::sprinklePlants(int numPlants){
	for(int i=0; i<numPlants; ++i){
		int position = rand() % (width*height);
		WorldCell &cell = cells[position];
		
		cell.plantGrowth = 0.01;
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
			std::cout << (cell.pet ? (cell.pet->energy > 0 ? '*' : 'o') : (cell.impassable ? 'X' : (cell.plantEnergy > 0 ? (cell.plantEnergy > 0.5 ? 'A' : '^') : ' ')));
			
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
	
	int currentPosition = petPositions[pet];

	// Eat plants.
	pet->energy += cells[currentPosition].plantEnergy;
	pet->energy -= 0.01;
	pet->energy = std::min(pet->energy, pet->maxEnergy);
	cells[currentPosition].plantEnergy = 0;

	if (pet->energy > 0) {
		Direction oldDirection = petDirections[pet];
		Direction newDirection = offsetDirectionByRelativeDirection(oldDirection, petIntention.relativeDirection);
		
		int newPosition = currentPosition;
		
		// Does the Pet want to move?
		if (petIntention.action && move) {
			
			// Calculate the target position.
			newPosition = movePosition(currentPosition, newDirection);
			
			// Is it possible to move there?
			if (!cells[newPosition].pet && !cells[newPosition].impassable) {
				
				// Actually move.
				
				cells[currentPosition].pet = NULL;
				cells[newPosition].pet = pet;
				
				petDirections[pet] = newDirection;
				petPositions[pet] = newPosition;
			}		
		}
	}
	
}