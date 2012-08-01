#ifndef PET
#define PET

class PetIntention;
class World;

#include <vector>

#include <SFML/Graphics.hpp>


class Pet{

public:
	
 	Pet();
 	Pet(Pet &a, Pet &b);
	PetIntention getPetIntentionInWorld(const World &world);

	sf::Color color;
	

//protected:

	// Neural network stuff.

	void collectSensoryInput(const World &world, float inputNeurons[]);
	static void differenceAndSumFromAbsolutes(float &a, float &b);
	static void absolutesFromdifferenceAndSum(float &s1, float &s2);
	void processInput(float inputNeurons[], float outputNeurons[]);

	static const int numTouchingCells;
	static const int numTouchingCellChannels;
	static const int numTouchingCellInputs;
//	static const int numSymetricInputs;

	static const int numInputNeurons;
	static const int numLayers;
	static const int layerSize;
	static const int numOutputNeurons;

	std::vector<std::vector<float> > inputConnections;
	std::vector<std::vector<std::vector<float> > > processingConnections;
	std::vector<std::vector<float> > outputConnections;


	static float geneticDistance(const Pet &a, const Pet &b);

};

#endif