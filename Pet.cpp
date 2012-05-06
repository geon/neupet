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
const int Pet::numInputNeurons = numRelativeDirections;
const int Pet::numOutputNeurons = numRelativeDirections;


Pet::Pet() : energy(0.5), maxEnergy(1) {
	
	// Initialize all neuron connections randomly.
	
	inputConnections.resize(layerSize);
	for (int i = 0; i < layerSize; ++i) {
		inputConnections[i].resize(numInputNeurons);
		for (int j = 0; j < numInputNeurons; ++j) {
			inputConnections[i][j] = (rand() / (float) RAND_MAX) * 2 - 1;
		}
	}
	
	outputConnections.resize(layerSize);
	for (int i = 0; i < layerSize; ++i) {
		outputConnections[i].resize(numOutputNeurons);
		for (int j = 0; j < numOutputNeurons; ++j) {
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
}

/*
Pet::Pet(Pet &a, Pet &b) {
	
	// Cross parents a and b.
	
	inputConnections.resize(layerSize);
	for (int i = 0; i < layerSize; ++i) {
		inputConnections[i].resize(numInputNeurons);
		for (int j = 0; j < numInputNeurons; ++j) {
			float rand() / (float) RAND_MAX
			inputConnections[i][j] = ;
		}
	}
	
	outputConnections.resize(layerSize);
	for (int i = 0; i < layerSize; ++i) {
		outputConnections[i].resize(numOutputNeurons);
		for (int j = 0; j < numOutputNeurons; ++j) {
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
	
}
*/

PetIntention Pet::getPetIntentionForSensoryInput(SensoryInput sensory){

	// Collect data into an easy to access array.
	float inputNeurons[numRelativeDirections];
	for (RelativeDirection relativeDirection = firstRelativeDirection; relativeDirection < numRelativeDirections; ++relativeDirection) {
		
		inputNeurons[relativeDirection] = sensory.touchedCells[relativeDirection].pet ? 1 : 0;
		
	}
	
	// Make the input symmetric.
	/*
	 
	             b c 
	 forward -> a * d
	             B C
	 
	 Instead of sampling b, B, c, C... I sample b-B, b+B, c-C, c+C... to make the nn more insensitive to mirroring.
	 This is done along both the left/right and forward/backward axis.
	 
	 */
	differenceAndSumFromAbsolutes(inputNeurons[forwardLeft],  inputNeurons[forwardRight]);
	differenceAndSumFromAbsolutes(inputNeurons[backwardLeft], inputNeurons[backwardRight]);
	differenceAndSumFromAbsolutes(inputNeurons[forwardLeft],  inputNeurons[backwardLeft]);
	differenceAndSumFromAbsolutes(inputNeurons[forwardRight], inputNeurons[backwardRight]);
	differenceAndSumFromAbsolutes(inputNeurons[forward],      inputNeurons[backward]);

	// Think it over.
	float outputNeurons[numRelativeDirections];
	processInput(inputNeurons, outputNeurons);

	// Translate from mirror-insensitive output to direct values.
	absolutesFromdifferenceAndSum(outputNeurons[forward],      outputNeurons[backward]);
	absolutesFromdifferenceAndSum(outputNeurons[forwardRight], outputNeurons[backwardRight]);
	absolutesFromdifferenceAndSum(outputNeurons[forwardLeft],  outputNeurons[backwardLeft]);
	absolutesFromdifferenceAndSum(outputNeurons[backwardLeft], outputNeurons[backwardRight]);
	absolutesFromdifferenceAndSum(outputNeurons[forwardLeft],  outputNeurons[forwardRight]);

	// Translate neuron output to PetIntention. (Find the neuron with the largest output. Each neuron represents a direction.)
	float maxOutput = FLT_MIN;
	RelativeDirection intendedRelativeDirection = forward;
	for (RelativeDirection relativeDirection = firstRelativeDirection; relativeDirection < numRelativeDirections; ++relativeDirection) {

		if (outputNeurons[relativeDirection] > maxOutput) {
			maxOutput = outputNeurons[relativeDirection];
			intendedRelativeDirection = relativeDirection;
		}
	}
	
	return PetIntention(static_cast<Action>(mate | battle | move), intendedRelativeDirection);
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
			processingNeuronsWrite[write] = inputNeurons[read] * inputConnections[write][read];
		}
	}
	
	// Propagate through the rest of the processing layers.
	for (int layer=1; layer<numLayers; ++layer) {

		// Reuse the two layers of neurons. Much like double-buffered graphics.
		std::swap(
				  processingNeuronsRead,
				  processingNeuronsWrite
				  );
	
		for (int write = 0; write < layerSize; ++write) {
			processingNeuronsWrite[write] = 0;
			for(int read = 0; read < layerSize; ++read)
				processingNeuronsWrite[write] += processingNeuronsRead[read] * processingConnections[layer][write][read];
		}
	}
	
	// Output neurons read from the last layer.
	std::swap(
			  processingNeuronsRead,
			  processingNeuronsWrite
			  );
	for (int write = 0; write < numOutputNeurons; ++write) {
		outputNeurons[write] = 0;
		for (int read = 0; read < layerSize; ++read) {
			outputNeurons[write] = processingNeuronsRead[read] * outputConnections[write][read];
		}
	}
}




//	const int numLayers = 1; // Arbitrary
//	const int layerSize = 4; // Arbitrary
//
//	const int numInputPositions = 7; // All positions within 1 steps.
//	const int numInputChannels = 3; // Other pets mate-abillity, nutricient, danger.
//	const int numOutputPositions = 7; // All positions within 1 steps.
//	const int numGeneralOutputs = 2; // Mate, scent.
//
//	const int numInputNeurons = numInputPositions*numInputChannels;
//	const int numProcessingNeurons = numLayers*layerSize;
//	const int numOutputNeurons = numOutputPositions + numGeneralOutputs;
//
//	const int numInputConnections = numInputPositions*numInputChannels*layerSize;
//	const int numProcessingConnections = (numLayers-1)*layerSize*layerSize;
//	const int numOutputConnections = (numOutputPositions + numGeneralOutputs)*layerSize;
//
//	float inputNeurons[numInputNeurons];
//	float processingNeurons[numProcessingNeurons];
//	float outputNeurons[numOutputNeurons];
//
//	float inputConnections[numinputConnections];
//	float processingConnections[numProcessingConnections];
//	float outputConnections[numOutputConnections];


//	void collectInput(){
//
//		for(Channels channel = 0; i<numChannels; ++i)
//			for(Directions direction = 0; i<numDirections; ++i)
//				inputNeurons[channel*numDirections + direction] = world.sample(position, facingDirection, direction, channel);
//	}


