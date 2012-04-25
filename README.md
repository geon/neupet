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

* Pets should be presented a pre-rotated and filtered piece of the world they can sense. No access to the world needed.

### World/Pet Sophistication

* Life limit and energy consumption etc.
* Plants
* Water (with Pets able to traverse water or land by varying speed? Value for how land or water-bound they are? Some Pets amphibious. How to ballance it?)
* Battles & Mating - Add a flag for each so the Pet can signal if it is violent or "in heat". Violent Pets will battle anything it moves into. Pets in heat will mate if running into another Pet in heat.
* Make Pets sense the direction other Pets are facing. This will enable flocking.
* Neural network.
* WorldCell height - (Replaces impassable walls and water (water level = 0).) Higher energy consumption uphill, impossible to climb too steep hills (gradient > 1), dangerous drops. (Needs 3D (or 2.5D) graphics.)
* Speciation. Add an input for how many genes a pair of Pets share.
* New senses.
	* Sight (1/distance to closest Pet) in n directions over m degrees of FOV. Multiple channels for food, mate and danger?
	* Smell - Each world cell should have the smell of the last occupant. Outputs to the visiting Pet is the strenght of the smell, and the gene similarity to itself. (?)

### Misc.

* Pretty graphics.
* Ancestry tracking. Useful for statistics.
