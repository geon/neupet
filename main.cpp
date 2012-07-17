#include "World.h"

#include <iostream>
#include <stdlib.h>
#include <time.h>

#include <SFML/Graphics.hpp>

int main()
{
	

	
	srand(time(NULL));
	
	World world;
	
	//	std::cout << "How many pets? ";
	//	int numPets;
	//	std::cin >> numPets;
	//	world.generatePopulation(numPets);
	
	world.sprinklePlants(3000);
	
	
	
	
	// Create main window
    sf::RenderWindow App(sf::VideoMode(640, 480), "neupet");
	
    // Start game loop
    while (App.IsOpened())
    {
        // Process events
        sf::Event Event;
        while (App.GetEvent(Event))
        {
            // Close window : exit
            if (Event.Type == sf::Event::Closed)
                App.Close();
        }
		
        // Clear screen
        App.Clear();
		

		world.step();
		world.render(App);
		
		
        // Finally, display the rendered frame on screen
        App.Display();
    }
	
    return EXIT_SUCCESS;
}