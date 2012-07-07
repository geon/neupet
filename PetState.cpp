#include "PetState.h"


const float PetState::liveEnergy = 1.0/200;
const float PetState::moveEnergy = 1.0/100;
const float PetState::breedEnergy = 0.5;
const int PetState::maxAge = 1000;

PetState::PetState() : energy(0.5), maxEnergy(1), age(0) {
}


PetState::PetState(int position, Direction direction, float energy) : position(position), direction(direction), energy(energy), maxEnergy(1), age(0) {
}


bool PetState::isAlive() {
	
	// The pet is alive while its is young and strong enough.
	return age <= maxAge && energy > 0;
}