#ifndef PET
#define PET

class PetIntention;
class SensoryInput;


class Pet{

public:
	
 	Pet() : energy(0.5), maxEnergy(1) {}
	PetIntention getPetIntentionForSensoryInput(SensoryInput sensory);
	
	
//protected:
	
//	float breedingEnergy = 0.2;
	float energy;
	float maxEnergy;
};

#endif