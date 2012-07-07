#include "World.h"

#include <iostream>
#include <stdlib.h>
#include <time.h>


int main()
{
	
	srand(time(NULL));

	std::cout << "How many pets? ";
	int numPets;
	std::cin >> numPets;

	World world;
//	world.generatePopulation(numPets);
	world.sprinklePlants(100);
	world.render();

	
	while(char command = std::cin.get() != 'q'){

		int stepsPerFrame = 1;
		if (command == 'c') {
			stepsPerFrame = 100;
		}
		if (command == 'm') {
			stepsPerFrame = 1000;
		}
		if (command == 'v') {
			stepsPerFrame = 10000;
		}
		if (command == 'b') {
			stepsPerFrame = 100000;
		}
		
		for (int i = 0; i < stepsPerFrame; ++i) {
			world.step();
		}

		world.render();
	}

	return 0;
}