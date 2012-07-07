NeuPet
======

Evolving neural networks-controlled virtual pets -- a petri dish in your computer.

Why?
----

I'm very faschinated by evolutionary algritms, virtual life and artificial intelligence. This project combines them, and since I had the iea, it has been irresistable to me.

NeuPet is heavily inspired by a MS-DOS game called [Evolve! Lite](http://www.youtube.com/watch?v=kPQBexmrvEQ) I found on a shareware CD in the nineties, called Game Empire. It used pseudo celluar automata to manage a population of "creatures" moving around on a grid, acording to a configurable set of rules. The creatures would respond to the prescense of friends or foes (hardcoded as two spicies) and go forward, left, right, etc.

How?
----

A set of "pets" live in a hexagonal grid world. Each pet is controlled by a neural network, and has to find food and avoid getting eaten in order to survive and reproduce.


TODO
----

### World/Pet Interaction

* Make the world generate a new population from the last n surviving pets.
* Change the PetState::maxAge to a genetic variable, or PetState::maxEnergy to a constant. Figure out how they should be balanced/capped.
* Interface for the Pet position and direction, energy levels etc. in World.
* Split the plant logic to separate class with generic base class. (?) Not fun.

### World/Pet Sophistication

* Health, Strength, Metabolism, Mass etc. interacting with each other. Dead Pets should still contain some energy to be useful as food.
* Battles & Mating - Add a flag for each so the Pet can signal if it is violent or "in heat". Violent Pets will battle anything it moves into. Pets in heat will mate if running into another Pet in heat.
* Bundle the Pet and PetState into a common stuct for storage in World. Makes the std::map from Pet to PetState unnecesary.
* Decompose dead pets slowly.
* Decomposing Pets should seep into the ground, adding nutricients (pland growth rate) for plants.
* Plants should deplete the nutricients of the ground.
* Pooping Pets add fertilizer to the ground?
* Make Pets sense the direction other Pets are facing. This will enable flocking.
* Water (with Pets able to traverse water or land by varying speed? Value for how land or water-bound they are? Some Pets amphibious. How to ballance it?)
* WorldCell height - (Replaces impassable walls and water (water level = 0).) Higher energy consumption uphill, impossible to climb too steep hills (gradient > 1), dangerous drops. (Needs 3D (or 2.5D) graphics.)
* Speciation. Add an input for how many genes a pair of Pets share.
* New senses.
	* Sight (1/distance to closest Pet) in n directions over m degrees of FOV. Multiple channels for food, mate, wall and danger?
	* Smell - Each world cell should have the smell of the last occupant. Outputs to the visiting Pet is the strenght of the smell, and the gene similarity to itself. (?)

### Misc.

* Non-neural Pet class until I get the world and evolution working.
* Graphics.
* Pretty graphics. http://www.youtube.com/watch?v=-PsQqpK0MvM&list=UUutMBmI_ydvgmzbkQxbI4IQ&index=9&feature=plcp
* GUI. Picking and examining Pets, etc.
* Ancestry tracking. Useful for statistics.


### Life "Rules"

This will be impotant to make the simulation sensislbe. They are the laws of physics in the World.

#### Genetically controlled values

* birthIdealMass - Mass of Pet at birth.
* targetIdealMass - Mass of fully grown Pet.
* growthRate
* childrenPerBirth

#### Simulation controlled values

* energyConversionFactor = 10
* breedEnergy - birthIdealMass * childrenPerBirth * energyConversionFactor
* idealMass - 
* maxMass = Target+50%, minMass )
* mass - (Mass gained by eating = mass of food / energyConversionFactor)

#### Calculated values

* Energy content/nutriciency => directly proportional to Mass.
* Greater mass => longer pregnancy (directly proportional, plus constant) http://www.applet-magic.com/gestation.htm
* Greater mass => greater strength (mass^1/3 proportional to strength^1/2 ?)
