#ifndef WORLD
#define WORLD

#include "Direction.h"
#include "RelativeDirection.h"
#include "Action.h"

#include "WorldCell.h"
#include "Pet.h"
#include "PetIntention.h"
#include "PetState.h"

#include <list>
#include <map>

class World{

	public:

	World();
	~World();
	
	static const int width = 30;
	static const int height = 20;

//	protected:

	WorldCell cells[width*height];
	std::list<Pet *> pets;
	std::map<Pet *, PetState> petStates;
	
	bool addPet(Pet *newPet, PetState const &newState);
	void removePet(Pet *pet);
	
	public:

	int coordinateToIndex(int x, int y);
	int movePosition(int position, Direction direction);
	Direction offsetDirectionByRelativeDirection(Direction direction, RelativeDirection relativeDirection);

	void buildCage(int sideLength);
	void generatePopulation(int numPets);
	void sprinklePlants(int numPlants);
	int addPetAndPlaceRandomly(Pet *newPet, PetState newState);
	void render();
	void step();
	void applyPetIntentionToPet(Pet *pet, PetIntention petIntention);
};

#endif