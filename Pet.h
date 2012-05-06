#ifndef PET
#define PET

class PetIntention;
class SensoryInput;

#include <vector>


class Pet{

public:
	
 	Pet();
// 	Pet(Pet &a, Pet &b);
	PetIntention getPetIntentionForSensoryInput(SensoryInput sensory);

	void collectInput(SensoryInput sensory);
	
	
//protected:

	static void differenceAndSumFromAbsolutes(float &a, float &b);
	static void absolutesFromdifferenceAndSum(float &s1, float &s2);
	void processInput(float inputNeurons[], float outputNeurons[]);

	// Neural network stuff.
	static const int numLayers;
	static const int layerSize;
	static const int numInputNeurons;
	static const int numOutputNeurons;
	
	std::vector<std::vector<float> > inputConnections;
	std::vector<std::vector<std::vector<float> > > processingConnections;
	std::vector<std::vector<float> > outputConnections;
	
//	float breedingEnergy = 0.2;
	float energy;
	float maxEnergy;
};

#endif