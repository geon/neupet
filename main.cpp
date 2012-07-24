#include "World.h"

#include <iostream>
#include <stdlib.h>
#include <time.h>

#include <SFML/Graphics.hpp>

int main()
{
	

	
	srand(time(NULL));
	
	World world;
	

	
	
	
	// Create main window
    sf::RenderWindow App(sf::VideoMode(640, 480), "neupet");
	App.Clear();
	
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


			if (Event.Type == sf::Event::KeyPressed) {
				
				if (Event.Key.Code == sf::Key::S) {

					// Run n populations.
					for (int i = 0; i< 10; ++i) {
						
						// Run until the population goes extiinct.
						while (world.step()) {
							
							// Just run the simulation invisibly.
						}
						
						world.regeneratePopulation();
					}
				}
			}
        }
		
		// Run the simulation until the population goes extinct.
		if (!world.step()) {

			// ...then regenerate it.
			world.regeneratePopulation();
		}

		// Show the world.
		world.render(App);
		App.Display();
	}
	
    return EXIT_SUCCESS;
}