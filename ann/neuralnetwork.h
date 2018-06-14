#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include "../dataReader/datareader.h"
#include "layer.h"

using namespace arma;
using namespace parameters;

class Trainer;
class BatchTrainer;

class NeuralNetwork
{
public:
    int netType;
    bool isTraining;
    NeuralNetwork(vector<Layer*> nlayers, int type);
    NeuralNetwork(string weightFileName);
    bool saveWeights(string outputFilename);
    mat feedForwardPattern(arma::mat input);
    double getSetAccuracy( std::vector<DataEntry*>& set );
    vector<double> getSetAccuracyForEachClass(vector<DataEntry*>& set);
    double getSetMSE(vector<DataEntry*> &set);
    arma::mat clampOutput();
    void updateWeights(vector<mat> deltaWeights, vector<mat> deltaBiass, double learningRate);
    mat getOutput();
    void printNetwokInfo();
    mat getVisualizeOutput(mat input);
    vector<string> getNeuralInfoForVisualize();
    vector<Layer*> getLayers();
private:
    int nLayer;
    //neurons
    vector<Layer*> layers;
    //weights
    vector<mat> weights;
    //bias
    vector<mat> biass;

    friend Trainer;
    friend BatchTrainer;

    bool checkOutput(mat output, mat target);
    mat initializeWeights(int nRows, int nCols);
};

#endif // NEURALNETWORK_H
