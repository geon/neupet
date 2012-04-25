#include <iostream>
#include <list>
#include <map>

#include <stdlib.h>
#include <time.h>

// using namespace std;


/*

	f   G   H
  g   b   C   I
h	c  a  D   J
  i   d   e   K
	j   k   l


	a
	b-B
	b+B
	c-C
	c+C
	...

	Instead of sampling b, B, c, C... I sample b-B, b+B, c-C, c+C... to make the nn insensitive to direction.
	HANDLED BY World::Sample.
	
*/



enum RelativeDirection {
	forward,
	forwardRight,
	backwardRight,
	backward,
	backwardLeft,
	forwardLeft,
	numRelativeDirections
};


enum Direction {
	left,
	upLeft,
	upRight,
	right,
	downRight,
	downLeft,
	numDirections
};


enum Action {
	move   = 1 << 0,
	battle = 1 << 1,
	mate   = 1 << 2
};


struct PetIntention{
	
	PetIntention(Action action, RelativeDirection relativeDirection) : action(action), relativeDirection(relativeDirection) {
	}
	
	Action action;
	RelativeDirection relativeDirection;
};


struct Sensory{
	// This should contain everything the Pet can sense.
};




class Pet{

	public:
	
//	Pet(){
//
//	};

	protected:

//	const int numLayers = 1; // Arbitrary
//	const int layerSize = 4; // Arbitrary
//
//	const int numInputPositions = 7; // All positions within 1 steps.
//	const int numInputChannels = 3; // Other pets mate-abillity, nutricient, danger.
//	const int numOutputPositions = 7; // All positions within 1 steps.
//	const int numGeneralOutputs = 2; // Mate, scent.
//
//	const int numInputNeurons = numInputPositions*numInputChannels;
//	const int numProcessingNeurons = numLayers*layerSize;
//	const int numOutputNeurons = numOutputPositions + numGeneralOutputs;
//
//	const int numInputConnections = numInputPositions*numInputChannels*layerSize;
//	const int numProcessingConnections = (numLayers-1)*layerSize*layerSize;
//	const int numOutputConnections = (numOutputPositions + numGeneralOutputs)*layerSize;
//
//	float inputNeurons[numInputNeurons];
//	float processingNeurons[numProcessingNeurons];
//	float outputNeurons[numOutputNeurons];
//
//	float inputConnections[numinputConnections];
//	float processingConnections[numProcessingConnections];
//	float outputConnections[numOutputConnections];


//	void collectInput(){
//
//		for(Channels channel = 0; i<numChannels; ++i)
//			for(Directions direction = 0; i<numDirections; ++i)
//				inputNeurons[channel*numDirections + direction] = world.sample(position, facingDirection, direction, channel);
//	}


//	void processInput(){
//		// First layer takes input directly from input neurons.
//		for(int write=0; write<layerSize; ++write){
//			processingNeurons[write] = 0;
//			for(int read=0; read<numInputNeurons; ++read)
//				processingNeurons[write] = inputNeurons[read] * inputConnections[write*layerSize + read];
//		}
//
//		// Propagate through the rest of the processing layers.
//		for(int layer=1; layer<numLayers; ++layer)
//			for(int write=0; write<layerSize; ++write){
//				processingNeurons[layer*layerSize + write] = 0;
//				for(int read=0; read<layerSize; ++read)
//					processingNeurons[layer*layerSize + write] += processingNeurons[(layer-1)*layerSize + read] * processingConnections[write*layerSize + read];
//			}
//
//		// Output neurons read from the last layer.
//		for(int write=0; write<numOutputNeurons; ++write){
//			outputNeurons[write] = 0;
//			for(int read=0; read<layerSize; ++read)
//				outputNeurons[write] = processingNeurons[(numLayers-1)*layerSize + read] * outputConnections[read*layerSize + write];
//		}
//	}



//	float breedingEnergy = 0.2;
//	float energy;


	public:

	PetIntention getPetIntentionForSensory(Sensory sensory){
//		collectInput();
//		processInput();
//		respondToInput();

		RelativeDirection relativeDirection;

		float random = rand() / (float) RAND_MAX;

		if(random < .1) {
			relativeDirection = forwardLeft;
		} else if(random < .1+.1) {
			relativeDirection = forwardRight;
		} else if(random < .1+.1+.05) {
			relativeDirection = backwardLeft;
		} else if(random < .1+.1+.05+.05) {
			relativeDirection = backwardRight;
		} else if(random < .1+.1+.05+.05+.02) {
			relativeDirection = backward;
		} else {
			relativeDirection = forward;
		}

		return PetIntention(static_cast<Action>(mate | battle | move), relativeDirection);
	}


};


struct WorldCell{

	WorldCell(){

		pet = NULL;
		
		plantEnergy = 0;
		plantMaxEnergy = 1;
		plantGrowth = 0.01;
		
		impassable = false;
	}
	
	Pet *pet;
	
	float plantEnergy;
	float plantMaxEnergy;
	float plantGrowth;
	
	bool impassable;
};


class World{

	public:

	World(){

		buildCage(6);
	}
	~World(){

		for(std::list<Pet *>::iterator i=pets.begin(); i != pets.end(); ++i){
			delete *i;
		}

	}

	static const int width = 30;
	static const int height = 20;

	protected:

	WorldCell cells[width*height];
	std::list<Pet *> pets;
	std::map<Pet *, int> petPositions;
	std::map<Pet *, Direction> petDirections;
	
	public:

	int coordinateToIndex(int x, int y){
		return y*width + x;
	}

	int movePosition(int position, Direction direction){
		
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
	
	
	Direction offsetDirectionByRelativeDirection(Direction direction, RelativeDirection relativeDirection){
		
		return static_cast<Direction>(
			(static_cast<int>(direction) + static_cast<int>(numDirections) + static_cast<int>(relativeDirection)) % static_cast<int>(numDirections)
		);
	}


	void buildCage(int sideLength){
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


	void generatePopulation(int numPets){
		for(int i=0; i<numPets && i<width*height; ++i){
			Pet *pet = new Pet();;
			int position = i * (width*height)/(float)numPets;
			cells[position].pet = pet;
			pets.push_back(pet);
			petPositions[pet] = position;
			petDirections[pet] = static_cast<Direction>(rand() % numDirections);
		}
	}


	void render(){

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
				std::cout << (cell.pet ? '*' : (cell.impassable ? 'X' : ' '));

				// Spacing for hexagonal layout.
				std::cout << ' ';
			}
		}

	}


	void step(){

		for(std::list<Pet *>::iterator i=pets.begin(); i != pets.end(); ++i){
			applyPetIntentionToPet(*i, (*i)->getPetIntentionForSensory(Sensory()));
		}
		
	}
	
	
	void applyPetIntentionToPet(Pet *pet, PetIntention petIntention){
	
		Direction oldDirection = petDirections[pet];
		Direction newDirection = offsetDirectionByRelativeDirection(oldDirection, petIntention.relativeDirection);

		int oldPosition = petPositions[pet];
		int newPosition = oldPosition;
		
		// Does the Pet want to move?
		if (petIntention.action && move) {
			
			// Calculate the target position.
			newPosition = movePosition(oldPosition, newDirection);

			// Is it possible to move there?
			if (!cells[newPosition].pet && !cells[newPosition].impassable) {
				
				// Actually move.
				
				cells[oldPosition].pet = NULL;
				cells[newPosition].pet = pet;
		
				petDirections[pet] = newDirection;
				petPositions[pet] = newPosition;
			}		
		}

	}


};



int main()
{
	
	srand(time(NULL));

	std::cout << "How many pets? ";
	int numPets;
	std::cin >> numPets;

	World world;
	world.generatePopulation(numPets);
	world.render();

	while(std::cin.get() != 'q'){
		world.step();
		world.render();
	}

	return 0;
}




