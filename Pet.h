#ifndef PET
#define PET

class PetIntention;
class SensoryInput;


class Pet{

	protected:
	

//	float breedingEnergy = 0.2;
//	float energy;


	public:

	PetIntention getPetIntentionForSensoryInput(SensoryInput sensory);

};

#endif