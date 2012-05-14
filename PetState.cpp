#include "PetState.h"

PetState::PetState() : energy(0.5), maxEnergy(1) {
}

PetState::PetState(int position, Direction direction, float energy) : position(position), direction(direction), energy(energy), maxEnergy(1) {
}