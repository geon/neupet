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


/*
enum FacingDirection {
	none,
	forward,
	forwardLeft,
	forwardRight,
	backward,
	backwardLeft,
	backwardRight,
	numDirections
};
*/
enum Direction {
	none,
	left,
	upLeft,
	upRight,
	right,
	downRight,
	downLeft,
	numDirections
};

class World;

class Pet{

	public:
	
	Pet(World &world):world(world){

		facingDirection = static_cast<Direction>(rand() % numDirections);
	};

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


	World &world;
	
//	int position;
	Direction facingDirection;

//	float breedingEnergy = 0.2;
//	float energy;


	public:

	void live();


};


class World{

	public:

	World(){

		for(int i=0; i<width*height; ++i){
			cells[i] = NULL;
		}
	}
	~World(){

//		for(int i=0; i<width*height; ++i){
//			delete cells[i];
//		}

		for(std::list<Pet *>::iterator i=pets.begin(); i != pets.end(); ++i){
			delete *i;
		}

	}

	static const int width = 30;
	static const int height = 20;

	protected:

	Pet *cells[width*height];
	std::list<Pet *> pets;
	std::map<Pet *, int> petPositions;
	
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
			
		x %= width;
		y %= height;

		return coordinateToIndex(x, y);
	}

	void generatePopulation(int numPets){
		for(int i=0; i<numPets && i<width*height; ++i){
			Pet *pet = new Pet(*this);;
			int position = i * (width*height)/(float)numPets;
			cells[position] = pet;
			pets.push_back(pet);
			petPositions[pet] = position;
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
				Pet *cell = cells[coordinateToIndex(x, y)];

				// Mark any occupied cell.
				std::cout << (cell ? '*' : ' ');

				// Spacing for hexagonal layout.
				std::cout << ' ';
			}
		}

	}


	void step(){

		for(std::list<Pet *>::iterator i=pets.begin(); i != pets.end(); ++i){
			(*i)->live();
		}
		
	}
	
	
	void movePet(Pet &pet, Direction direction){
	
		int oldPosition = petPositions[&pet];
		int newPosition = movePosition(oldPosition, direction);

		if (!cells[newPosition]) {
			cells[oldPosition] = NULL;
			cells[newPosition] = &pet;
	
			petPositions.erase(&pet);
			petPositions[&pet] = newPosition;
		}		
	}


};


void Pet::live(){
//		collectInput();
//		processInput();
//		respondToInput();


	if(! (rand() % 20))
		facingDirection = static_cast<Direction>(rand() % numDirections);


	world.movePet(*this, facingDirection);
}



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




