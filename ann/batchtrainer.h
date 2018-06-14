#ifndef BATCHTRAINDER_H
#define BATCHTRAINDER_H

#include "neuralnetwork.h"
#include "trainer.h"

#define LAMDA 0

using namespace arma;

class BatchTrainer: public Trainer{
private:
    int batchSize;
    //learning parameters
    double learningRate;					// adjusts the step size of the weight update

    //epoch counter
    long epoch;
    long maxEpochs;

    //accuracy/MSE required
    double desiredAccuracy;
    double desiredMSE;
    //change to weights

    NeuralNetwork* NN;

    vector<mat> sumDeltaWeights;
    vector<mat> sumDeltaBiass;


public:
    BatchTrainer( NeuralNetwork* untrainedNetwork, int batchSize );
    BatchTrainer(NeuralNetwork* untrainedNetwork, double _learningRate, int _epoch, double _desiredMSE, int _batchSize );
private:
    void backpropagate( mat desiredOutputs,int index );
    void resetSumDelta();
    void runTrainingEpoch( std::vector<DataEntry*> trainingSet );
    inline void updateWeights();
};

#endif // BATCHTRAINDER_H
