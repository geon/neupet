#include "Pet.h"

#include "Direction.h"
#include "RelativeDirection.h"
#include "Action.h"

#include "WorldCell.h"
#include "PetIntention.h"
#include "SensoryInput.h"

#include <stdlib.h>
#include <cfloat>
#include <algorithm>


const int Pet::numLayers = 1;
const int Pet::layerSize = 6;
const int Pet::numInputNeurons = numRelativeDirections+1;
const int Pet::numOutputNeurons = numRelativeDirections+3;


Pet::Pet() {
	
	// Initialize all neuron connections randomly.
	
	inputConnections.resize(layerSize);
	for (int i = 0; i < layerSize; ++i) {
		inputConnections[i].resize(numInputNeurons);
		for (int j = 0; j < numInputNeurons; ++j) {
			inputConnections[i][j] = (rand() / (float) RAND_MAX) * 2 - 1;
		}
	}
	
	outputConnections.resize(numOutputNeurons);
	for (int i = 0; i < numOutputNeurons; ++i) {
		outputConnections[i].resize(layerSize);
		for (int j = 0; j < layerSize; ++j) {
			outputConnections[i][j] = (rand() / (float) RAND_MAX) * 2 - 1;
		}
	}
	
	processingConnections.resize(numLayers);
	for (int k = 0; k < numLayers; ++k) {
		processingConnections[k].resize(layerSize);
		for (int i = 0; i < layerSize; ++i) {
			processingConnections[k][i].resize(layerSize);
			for (int j = 0; j < layerSize; ++j) {
				processingConnections[k][i][j] = (rand() / (float) RAND_MAX) * 2 - 1;
			}
		}
	}
	
	
	// Set a random color.
	color = sf::Color(rand() / (float) RAND_MAX * 255,
					  rand() / (float) RAND_MAX * 255,
					  rand() / (float) RAND_MAX * 255);
}


Pet::Pet(Pet &a, Pet &b) {
	
	// Cross parents a and b.
	
	inputConnections.resize(layerSize);
	for (int i = 0; i < layerSize; ++i) {
		inputConnections[i].resize(numInputNeurons);
		for (int j = 0; j < numInputNeurons; ++j) {
			float factor = rand() / (float) RAND_MAX;
			inputConnections[i][j] = a.inputConnections[i][j] * factor + b.inputConnections[i][j] * (1-factor);
		}
	}
	
	outputConnections.resize(numOutputNeurons);
	for (int i = 0; i < numOutputNeurons; ++i) {
		outputConnections[i].resize(layerSize);
		for (int j = 0; j < layerSize; ++j) {
			float factor = rand() / (float) RAND_MAX;
			outputConnections[i][j] = a.outputConnections[i][j] * factor + b.outputConnections[i][j] * (1-factor);
		}
	}
	
	processingConnections.resize(numLayers);
	for (int k = 0; k < numLayers; ++k) {
		processingConnections[k].resize(layerSize);
		for (int i = 0; i < layerSize; ++i) {
			processingConnections[k][i].resize(layerSize);
			for (int j = 0; j < layerSize; ++j) {
				float factor = rand() / (float) RAND_MAX;
				processingConnections[k][i][j] = a.processingConnections[k][i][j] * factor + b.processingConnections[k][i][j] * (1-factor);
			}
		}
	}
	
	float factor = rand() / (float) RAND_MAX;
	color = sf::Color(a.color.r * factor + b.color.r * (factor - 1),
					  a.color.g * factor + b.color.g * (factor - 1),
					  a.color.b * factor + b.color.b * (factor - 1));

	
	
	// Add mutations.
	
	const float mutationRate = 0.01;

	for (int i = 0; i < layerSize; ++i) {
		for (int j = 0; j < numInputNeurons; ++j) {
			if (rand() / (float) RAND_MAX < mutationRate) {
				inputConnections[i][j] = (rand() / (float) RAND_MAX) * 2 - 1;
			}
		}
	}
	
	for (int i = 0; i < numOutputNeurons; ++i) {
		for (int j = 0; j < layerSize; ++j) {
			if (rand() / (float) RAND_MAX < mutationRate) {
				outputConnections[i][j] = (rand() / (float) RAND_MAX) * 2 - 1;
			}
		}
	}
	
	for (int k = 0; k < numLayers; ++k) {
		for (int i = 0; i < layerSize; ++i) {
			for (int j = 0; j < layerSize; ++j) {
				if (rand() / (float) RAND_MAX < mutationRate) {
					processingConnections[k][i][j] = (rand() / (float) RAND_MAX) * 2 - 1;
				}
			}
		}
	}

	if (rand() / (float) RAND_MAX < mutationRate/100) {
		color = sf::Color(rand() / (float) RAND_MAX * 255,
						  rand() / (float) RAND_MAX * 255,
						  rand() / (float) RAND_MAX * 255);
	}
}


PetIntention Pet::getPetIntentionForSensoryInput(SensoryInput sensory){

	// Collect data into an easy to access array.
	float inputNeurons[numInputNeurons];
	for (RelativeDirection relativeDirection = firstRelativeDirection; relativeDirection < numRelativeDirections; ++relativeDirection) {
		
		inputNeurons[relativeDirection] = ((bool) sensory.touchedCells[relativeDirection].pet | sensory.touchedCells[relativeDirection].impassable) ? 1 : 0;
	}
	inputNeurons[numRelativeDirections] = 1; // The last cell is allways 1, to give the nn something to use when there is no other input.
	
	// Make the input symmetric.
	/*
	 
	             b c 
	 forward -> a * d
	             B C
	 
	             a A 
	 forward -> b * B
	             c C
	 
	 Instead of sampling b, B, c, C... I sample b-B, b+B, c-C, c+C... to make the nn more insensitive to mirroring.
	 This is done along both the left/right and forward/backward axis.
	 
	 */
	differenceAndSumFromAbsolutes(inputNeurons[forwardLeft],  inputNeurons[forwardRight]);
	differenceAndSumFromAbsolutes(inputNeurons[backwardLeft], inputNeurons[backwardRight]);
	differenceAndSumFromAbsolutes(inputNeurons[forwardLeft],  inputNeurons[backwardLeft]);
	differenceAndSumFromAbsolutes(inputNeurons[forwardRight], inputNeurons[backwardRight]);
	differenceAndSumFromAbsolutes(inputNeurons[forward],      inputNeurons[backward]);

	// Think it over.
	float outputNeurons[numOutputNeurons];
	processInput(inputNeurons, outputNeurons);

	// Translate from mirror-insensitive output to direct values.
	absolutesFromdifferenceAndSum(outputNeurons[forward],      outputNeurons[backward]);
	absolutesFromdifferenceAndSum(outputNeurons[forwardRight], outputNeurons[backwardRight]);
	absolutesFromdifferenceAndSum(outputNeurons[forwardLeft],  outputNeurons[backwardLeft]);
	absolutesFromdifferenceAndSum(outputNeurons[backwardLeft], outputNeurons[backwardRight]);
	absolutesFromdifferenceAndSum(outputNeurons[forwardLeft],  outputNeurons[forwardRight]);

	// Translate neuron output to RelativeDirection. (Find the neuron with the largest output. Each neuron represents a direction.)
	float maxOutput = -FLT_MAX;
	RelativeDirection intendedRelativeDirection = forward;
	for (RelativeDirection relativeDirection = firstRelativeDirection; relativeDirection < numRelativeDirections; ++relativeDirection) {

		if (outputNeurons[relativeDirection] > maxOutput) {
			maxOutput = outputNeurons[relativeDirection];
			intendedRelativeDirection = relativeDirection;
		}
	}
	
	
//	intendedRelativeDirection = (RelativeDirection) (rand() % numRelativeDirections);
	

	// Decide the action based on the 3 neurons after the drections.
	Action intendedAction = static_cast<Action>((0 < outputNeurons[numRelativeDirections] + 0 ? move   : 0)|
												(0 < outputNeurons[numRelativeDirections] + 1 ? battle : 0)|
												(0 < outputNeurons[numRelativeDirections] + 2 ? mate   : 0));
	
	return PetIntention(intendedAction, intendedRelativeDirection);
}


void Pet::differenceAndSumFromAbsolutes(float &a, float &b) {
	
	float s1 = a - b;
	float s2 = a + b;
	
	a = s1;
	b = s2;
}

void Pet::absolutesFromdifferenceAndSum(float &s1, float &s2) {
	
	float a = (s1 + s2) / 2;
	float b = (s2 - s1) / 2;
	
	s1 = a;
	s2 = b;
}


void Pet::processInput(float inputNeurons[], float outputNeurons[]) {

	float processingNeuronsA[layerSize];
	float processingNeuronsB[layerSize];
	
	float *processingNeuronsRead = processingNeuronsA;
	float *processingNeuronsWrite = processingNeuronsB;
	
	// First layer takes input directly from input neurons.
	for (int write = 0; write < layerSize; ++write) {
		processingNeuronsWrite[write] = 0;
		for (int read = 0; read < numInputNeurons; ++read) {
			processingNeuronsWrite[write] += inputNeurons[read] * inputConnections[write][read];
		}
	}
	
	// Reuse the two layers of neurons. Much like double-buffered graphics.
	std::swap(
			  processingNeuronsRead,
			  processingNeuronsWrite
			  );

	// Propagate through the rest of the processing layers.
	for (int layer=1; layer<numLayers; ++layer) {
	
		for (int write = 0; write < layerSize; ++write) {
			processingNeuronsWrite[write] = 0;
			for(int read = 0; read < layerSize; ++read)
				processingNeuronsWrite[write] += processingNeuronsRead[read] * processingConnections[layer][write][read];
		}

		// Reuse the two layers of neurons. Much like double-buffered graphics.
		std::swap(
				  processingNeuronsRead,
				  processingNeuronsWrite
				  );
	}
		
	// Compute the output.
	for (int write = 0; write < numOutputNeurons; ++write) {
		outputNeurons[write] = 0;
		for (int read = 0; read < layerSize; ++read) {
			outputNeurons[write] += processingNeuronsRead[read] * outputConnections[write][read];
		}
	}
}





