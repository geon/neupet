#include "World.h"

#include <iostream>
#include <algorithm>
#include <list>


// Must be defined in the header if they are used there.
//const int World::width = 30;
//const int World::height = 20;
//const int World::initialPopulationSize = 30;


World::World(){
	
	buildCage(20, upLeft);
	buildCage(35, downRight);
	buildCage(50, upLeft);
	buildCage(65, downRight);
	buildCage(80, upLeft);
	buildCage(95, downRight);
	buildWalls();

	sprinklePlants(3000);
	regeneratePopulation();

	image.Create(width*2+1, height*2);
}


World::~World() {
	
	for (std::list<Pet *>::iterator i=pets.begin(); i != pets.end(); ++i) {
		delete *i;
	}
	
	for (std::list<Pet *>::iterator i=petArchive.begin(); i != petArchive.end(); ++i) {
		delete *i;
	}
}


int World::coordinateToIndex(int x, int y) const {
	return y*width + x;
}


void World::indexToCoordinate(int position, int &x, int &y) const {
	
	x = position % width;
	y = position / width;
}


int World::movePosition(int position, Direction direction) const {
	
	int x, y;
	indexToCoordinate(position, x, y);
	
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
	
	// Make the wold tiling.
	x = (x + width) % width;
	y = (y + height) % height;
	
	return coordinateToIndex(x, y);
}


Direction World::offsetDirectionByRelativeDirection(Direction direction, RelativeDirection relativeDirection) const {
	
	return static_cast<Direction>(
								  (static_cast<int>(direction) + static_cast<int>(relativeDirection)) % static_cast<int>(numDirections)
								  );
}


RelativeDirection World::relativeDirectionBetweenDirections(Direction direction, Direction comparedToDirection) const {
	
	return static_cast<RelativeDirection>(
										  (static_cast<int>(direction) - static_cast<int>(comparedToDirection) + static_cast<int>(numDirections)) % static_cast<int>(numRelativeDirections)
										  );
}


void World::buildCage(int sideLength, Direction openingDirection) {
	// Draw a "cage" of walls with one side open.
	
	// Begin in the center.
	int position = coordinateToIndex(width/2, height/2);
	
	// Move to starting position
	for(int i=0; i<sideLength; ++i){
		position = movePosition(position, openingDirection);
	}
	cells[position].impassable = true;
	
	// Draw a semi circle. (5 sides of 6.)
	Direction direction = offsetDirectionByRelativeDirection(openingDirection, backwardLeft);
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

void World::buildWalls() {
	// Place walls along the sides of the world.
	
	// Place wall.
	for(int x=0; x<width; ++x){
		cells[x].impassable = true;
		cells[x + width*(height-1)].impassable = true;
	}
	for(int y=0; y<height; ++y){
		cells[y*width].impassable = true;
		cells[y*width + (width-1)].impassable = true;
	}
}


void World::regeneratePopulation() {

	// Remove any remaining Pet.
	while(pets.size()){
		removePet(pets.back());
	}	
	
	// Use archived pets for only half of ththe population, to weed out the bad ones.
	int maxNumPetsToReuse = initialPopulationSize / 2;
	if (petArchive.size() > maxNumPetsToReuse) {
		petArchive.resize(maxNumPetsToReuse);
	}
	
	// Generate Pets to fill up the initialPopulationSize.
	for(int i=0; i<initialPopulationSize && i<width*height; ++i){

		Pet *pet = 0;
		
		// Reuse the recently died Pets.
		if (!petArchive.empty()) {
			pet = petArchive.back();
			petArchive.pop_back();
		}
		
		// Create new ones if the archive is not large enough.
		if (!pet) {
			pet = new Pet();
		}
		
		if (0 > addPetAndPlaceRandomly(pet, PetState(0, static_cast<Direction>(rand() % numDirections), 1))) {
			
			// Stop if no more pets can fit.
			break;
		}
	}
	
	// Try not to introduce bias by the order in which Pets are processed.
	std::vector<Pet *> v(pets.begin(), pets.end());
	random_shuffle(v.begin(), v.end());
	pets = std::list<Pet *>(v.begin(), v.end());	
}


void World::sprinklePlants(int numPlants) {
	for(int position=0; position<width*height; ++position){

		
		WorldCell &cell = cells[position];

		float r1 = rand() / (float) RAND_MAX;
		float r2 = rand() / (float) RAND_MAX;
		int x, y;
		indexToCoordinate(position, x, y);
		
		
		cell.plantGrowth = sinf(x / 20.0) * sinf(y / 20.0) * 0.01 * cell.plantMaxEnergy - r1 * 0.1 + r2*r2*0.01;
		if (cell.plantGrowth <= 0) {
			cell.plantGrowth = 0;
		} else {
			cell.plantEnergy = cell.plantMaxEnergy;
		}
	}
}


void World::render(sf::RenderWindow &window) {
	
	for(int y=0; y < height; ++y){
		
		// Indent odd lines to line up hexagonally.
		int indentation = y % 2 ? 1: 0;
		
		for(int x=0; x < width; ++x){
			
			// Get the relevant cell.
			WorldCell &cell = cells[coordinateToIndex(x, y)];
			
			// Find a suitable color.
			sf::Color color = sf::Color::Black;
			if (cell.pet) {
				color = cell.pet->color;
			} else {
				if (cell.impassable) {
					color = sf::Color::White;
				} else {
					if (cell.plantEnergy > 0) {
						color = sf::Color(0, 255 * cell.plantEnergy, 0);
					}
				}
			}
			
			// Plot the cell.
			image.SetPixel(indentation + x*2 + 1, y*2    , color);			
			image.SetPixel(indentation + x*2    , y*2    , color);			
			image.SetPixel(indentation + x*2 + 1, y*2 + 1, color);			
			image.SetPixel(indentation + x*2    , y*2 + 1, color);			
		}
	}
	
	window.Draw(sf::Sprite(image));
}


bool World::step() {
	
	// Update Pets.
	std::list<Pet *>::iterator i = pets.begin();
	while (i != pets.end()) {
		
		Pet *pet = *i;

		// Advance i before applayPetIntention, so the loop won't crash when a Pet is erased.
		i++;
		
		applyPetIntentionToPet(pet, pet->getPetIntentionInWorld(*this));
	}
	
	// Update WorldCells.
	for (int i=0; i<width*height; ++i) {
		WorldCell &cell = cells[i];
		
		// Plant growth.
		cell.plantEnergy = std::min(cell.plantEnergy + cell.plantGrowth, cell.plantMaxEnergy);
	}

	// Is is the world still populated?
	return pets.size() > 1;
}


void World::applyPetIntentionToPet(Pet *pet, PetIntention petIntention) {
	
	PetState &currentState = petStates[pet];
	PetState newState = currentState;
	
	// Eat plants.
	newState.energy += cells[currentState.position].plantEnergy;
	cells[currentState.position].plantEnergy = 0;
	
	// Clamp the Pet's energy.
	newState.energy = std::min(newState.energy, PetState::maxEnergy);
	
	Direction oldDirection = currentState.direction;
	Direction newDirection = offsetDirectionByRelativeDirection(oldDirection, petIntention.relativeDirection);
	
	// Calculate the target position.
	int newPosition = movePosition(currentState.position, newDirection);
	
	// Does the Pet want to mate, and is it possible?
	if ((petIntention.action & mate) && cells[newPosition].pet && newState.energy > PetState::breedEnergy) {
		
		// Mate
		for (Direction direction = firstDirection; direction < numDirections; ++direction) {
			int neighbourPosition = movePosition(currentState.position, direction);
			WorldCell &cell = cells[neighbourPosition];
			if (!cell.pet && !cell.impassable) {
				Pet *child = new Pet(*pet, *(cells[newPosition].pet));
				addPet(child, PetState(neighbourPosition, direction, PetState::breedEnergy / 6));
			}
		}
		
		newState.energy -= PetState::breedEnergy;

	} else {

		// Does the Pet want to battle, and is it possible?
		if ((petIntention.action & battle)
			&& cells[newPosition].pet) {
			
			PetState &opponentState = petStates[cells[newPosition].pet];
			
			// If the Pet is stronger than the opponent, eat it.
			if (currentState.energy > opponentState.energy) {
				newState.energy -= PetState::battleEnergy;

				if (newState.isAlive()) {
					newState.energy	+= opponentState.energy;
					opponentState.energy = 0;
					//				removePet(cells[newPosition].pet);
					newState.energy = std::min(newState.energy, PetState::maxEnergy);
				}				
			}
			// Actually move.
			newState.direction = newDirection;
			newState.position = newPosition;
			cells[currentState.position].pet = 0;
			cells[newState.position].pet = pet;
			
			// Moving consumes some energy.
		}

		// Does the Pet want to move, and is it possible?
		if ((petIntention.action & move)
			&& !cells[newPosition].impassable
			&& !cells[newPosition].pet) {
			
			// Actually move.
			newState.direction = newDirection;
			newState.position = newPosition;
			cells[currentState.position].pet = 0;
			cells[newState.position].pet = pet;
			
			// Moving consumes some energy.
			newState.energy -= PetState::moveEnergy;
		}
	}
		
	// Just staying alive takes energy.
	newState.energy -= PetState::liveEnergy;
	
	// Age the pet.
	++newState.age;
	
	petStates[pet] = newState;
	
	// Handle death.
	if (!newState.isAlive()) {
		
		// For now, dying Pets just disappear.
		removePet(pet);
	}
}


int World::addPetAndPlaceRandomly(Pet *newPet, PetState newState) {

	// Walk over the list until a living Pet is found, max one lap. 
	int worldSize = width * height;
	int randomStartPosition = rand() % worldSize;
	for (int i = 0; i < worldSize; ++i) {

		int currentPosition = (randomStartPosition + i) % worldSize;

		// Try to place it here.
		newState.position = currentPosition;
		if (addPet(newPet, newState)) {

			return currentPosition;
		}
	}

	// No valid position was found, so return an invalid position.
	return -1;
}


bool World::addPet(Pet *newPet, PetState const &newState) {
	
	// Refuse to add Pets at any position already occupied by a Pet.
	if (cells[newState.position].pet) {
		return false;
	}
	
	// Refuse to add Pets at walls.
	if (cells[newState.position].impassable) {
		return false;
	}
	
	cells[newState.position].pet = newPet;
	pets.push_front(newPet); // push_front is OK, but not push_back, since the list "pets" is being itterated over while this function is called.
	petStates[newPet] = newState;
	
	return true;
}


void World::removePet(Pet *pet) {
	
	cells[petStates[pet].position].pet = 0;
	pets.remove(pet);
	petStates.erase(pet);
	
	petArchive.push_back(pet);
	
	// Keep the archive from growing larger than needed.
	while (petArchive.size() > initialPopulationSize / 2) {
		Pet *pet = petArchive.front();
		petArchive.pop_front();
		delete pet;
	}
}


