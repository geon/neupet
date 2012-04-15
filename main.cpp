#include <iostream>

using namespace std;


/*

    f   G   H
  g   b   C   I
h    c  a  D   J
  i   d   e   K
    j   k   l


    a
    b-B
    b+B
    c-C
    c+C
    ...

    Instead of sampling b, B, c, C... I sample b-B, b+B, c-C, c+C... to make the nn insensitive to direction.

*/



class Pet{

    protected:

    const int numLayers = 4; // Arbitrary
    const int layerSize = 10; // Arbitrary
    const int numInputPositions = 19; // All positions within 2 steps.
    const int numInputChannels = 6; // Other pets rgb, Scent rgb
    const int numOutputDirections = 6; // All directions
    const int numDirectionalOutputChannels = 3; // Move, Fight, Mate
    const int numGeneralOutputChannels = 3; // Null, Scent

    const int numInputNeurons = numInputPositions*numInputChannels;
    const int numProcessingNeurons = numLayers*layerSize;
    const int numOutputNeurons = numOutputDirections*numDirectionalOutputChannels + numGeneralOutputChannels;

    const int numInputConnections = numInputPositions*numInputChannels*layerSize;
    const int numProcessingConnections = (numLayers-1)*layerSize*layerSize;
    const int numOutputConnections = (numOutputDirections*numDirectionalOutputChannels + numGeneralOutputChannels)*layerSize;

    float inputNeurons[numInputNeurons];
    float processingNeurons[numProcessingNeurons];
    float outputNeurons[numOutputNeurons];

    float inputConnections[numinputConnections];
    float processingConnections[numProcessingConnections];
    float outputConnections[numOutputConnections];


    void collectInput(WorldCell environment[]){
        for(int i=0; i<)
    }
    void processInput(){
        // First layer takes input directly from input neurons.
        for(int write=0; write<layerSize; ++write){
            processingNeurons[write] = 0;
            for(int read=0; read<numInputNeurons; ++read)
                processingNeurons[write] = inputNeurons[read] * inputConnections[write*layerSize + read];
        }

        // Propagate through the rest of the processing layers.
        for(int layer=1; layer<numLayers; ++layer)
            for(int write=0; write<layerSize; ++write){
                processingNeurons[layer*layerSize + write] = 0;
                for(int read=0; read<layerSize; ++read)
                    processingNeurons[layer*layerSize + write] += processingNeurons[(layer-1)*layerSize + read] * processingConnections[write*layerSize + read];
            }

        // Output neurons read from the last layer.
        for(int write=0; write<numOutputNeurons; ++write){
            outputNeurons[write] = 0;
            for(int read=0; read<layerSize; ++read)
                outputNeurons[write] = processingNeurons[(numLayers-1)*layerSize + read] * outputConnections[read*layerSize + write];
        }
    }
    void respondToInput(){
        for(int i=0; i<)
    }


    const float breedingEnergy = 0.2;


    int direction;
    int position;
    float energy;
    float color[3];


    public:

    void live(){
        collectInput();
        processInput();
        respondToInput();
    }


}





int main()
{
    cout << "Hello world!" << endl;
    return 0;
}




