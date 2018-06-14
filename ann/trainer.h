#ifndef TRAINDER_H
#define TRAINDER_H

#include <fstream>
#include <vector>

//neural network header
#include "./neuralnetwork.h"

//Constant Defaults!
#define LEARNING_RATE 0.1

#define MOMENTUM 0.9
#define MAX_EPOCHS 100
#define DESIRED_ACCURACY 95
#define DESIRED_MSE 0.001

using namespace arma;
using namespace parameters;


class Trainer
{
private:

    //network to be trained
    NeuralNetwork* NN;

    //learning parameters
    double learningRate;// adjusts the step size of the weight update

    //epoch counter
    long epoch;
    long maxEpochs;

    //accuracy/MSE required
    double desiredAccuracy;
    double desiredMSE;

    //change to weights
    vector<mat> deltaWeights;
    vector<mat> deltaBiass;

    //error gradients
    vector<mat> errorGradients;

public:
    vector<double> trainingSetMSE;
    vector<double> validationSetMSE;

    vector<double> trainingSetAccuracy;
    vector<double> validationSetAccuracy;

    Trainer();
    Trainer( NeuralNetwork* untrainedNetwork );
    Trainer(NeuralNetwork* untrainedNetwork, double _learningRate,
                                            long _epoch,
                                            double _desireMSE);
    void trainNetwork(TrainingDataSet* tSet );
    bool checkOutput(mat output, mat target);
    bool isTraining();
private:
    virtual void runTrainingEpoch( std::vector<DataEntry*> trainingSet );
    void backpropagate(mat desiredOutputs);
    inline void updateWeights();
};

#endif // TRAINDER_H
