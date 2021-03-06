#include "neuralnetwork.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <math.h>


NeuralNetwork::NeuralNetwork(vector<Layer *> nlayers, int type):layers(nlayers), netType(type) {

    nLayer = (int)layers.size();
    for(int i =0; i < nLayer -1 ; i++){ // there are n-1 W between n layers
        int layerSize = layers[i]->nNeurals;
        int nextLayerSize = layers[i+1]->nNeurals;

        mat weight = initializeWeights(nextLayerSize, layerSize);
        weights.push_back(weight);

        mat bias;
        if(layers[i]->isBias){
            bias = initializeWeights(nextLayerSize, 1);
        }

        // if this layer has no bias => push empty(size = [0x0]); and Output layer has no bias.
        biass.push_back(bias);
    }

}

NeuralNetwork::NeuralNetwork(string weightFileName){
    ifstream weightFileStream(weightFileName);
    if(!weightFileStream.is_open()){
        cout<<"cannot open this file ! idiot !.\n";
    }
    else{
        weightFileStream>>nLayer;
        weightFileStream>>netType;

        for ( int i=0; i < nLayer; i++ ) {
            int nrs = 0, activefunc;
            bool isb;
            weightFileStream>>nrs;
            weightFileStream>>isb;
            weightFileStream>>activefunc;
            Layer *l = new Layer(nrs, isb, activefunc);
            layers.push_back(l);
        }

        for(int i=0; i< nLayer -1; i++){

            int nRows = layers[i+1]->nNeurals,
                nCols = layers[i]->nNeurals;
            mat weight = mat(nRows, nCols);
            weight.zeros();

            for(int r = 0; r< nRows; r++){
                for(int c =0; c< nCols; c++){
                    weightFileStream>>weight(r,c);
                }
            }
            weights.push_back(weight);
        }

        for(int i=0; i< nLayer -1; i++){
            mat bias;
            if(layers[i]->isBias){
                bias = initializeWeights(layers[i+1]->nNeurals, 1);
            }
            for(int b =0; b< layers[i+1]->nNeurals; b++){
                weightFileStream>>bias(b,0);
            }
            biass.push_back(bias);
        }

    }
}

mat NeuralNetwork::initializeWeights(int nRows, int nCols){
    mat weight;
    weight = mat(nRows, nCols);
    weight.zeros();

    for(int i=0; i< nRows; i++){
        for(int j=0; j< nCols; j++){
            weight(i, j) = gaussianRamdom(0, 0.5);
        }
    }

    return weight;
}

mat NeuralNetwork::feedForwardPattern(mat input){

    layers[0]->setNeuralsValue(input);
    layers[0]->activation();

    for(int i=1; i < nLayer; i++){

        mat tmp = weights[i-1] * layers[i-1]->neurals;
        if(biass[i - 1].n_cols != 0){
            tmp = tmp+ biass[i -1];
        }

        layers[i]->setNeuralsValue(tmp);
        layers[i]->activation();
    }
    return layers[nLayer - 1]->neurals;
}


mat NeuralNetwork::clampOutput(){ // this function is applied to classification

    mat res = layers[nLayer - 1]->neurals; // output layer

    for(int i=0 ;i< res.n_rows; i++){
        if(res(i,0) < 0.5){
            res(i,0) =0;
        }
        else if(res(i,0) >=0.5){
            res(i,0) =1;
        }
    }
    return res;
}

bool NeuralNetwork::checkOutput(mat output , mat target){
    int size = output.n_rows;
    for(int i =0; i<size; i++){
        if(abs(output(i,0) - target(i,0) ) > 0.001){
            return false;
        }
    }
    return true;
}

double NeuralNetwork::getSetAccuracy( std::vector<DataEntry*>& set ) {
    double incorrectResults = 0;

    //for every training input array
    int size = (int)set.size();
    for ( int tp = 0; tp < size; tp++) {
        mat input = set[tp]->pattern;
        //correct pattern flag
        bool correctResult = true;

        mat output = getVisualizeOutput(input);

        for(int i=0 ;i< output.n_rows; i++){
            if(output(i,0) < 0.5){
                output(i,0) =0;
            }
            else if(output(i,0) >=0.5){
                output(i,0) =1;
            }
        }

        //check all outputs against desired output values
        correctResult = checkOutput(output, set[tp]->target);

        //inc training error for a incorrect result
        if ( !correctResult ) incorrectResults++;

    }//end for

    //calculate error and return as percentage
    return 100 - (incorrectResults/set.size() * 100);
}

vector<double> NeuralNetwork::getSetAccuracyForEachClass(vector<DataEntry *> &set){
    if(set.size() == 0){
        return vector<double>(0);
    }
    int nClass = set[0]->target.n_rows;

    vector<double> correctResults(nClass, 0);
    vector<int> totalSample = voteOuput(set);
    int size = (int)set.size();
    for ( int tp = 0; tp < size; tp++) {
        mat input = set[tp]->pattern;
        mat output = getVisualizeOutput(input);
        for(int i=0 ;i< output.n_rows; i++){
            if(output(i,0) < 0.5){
                output(i,0) =0;
            }
            else if(output(i,0) >=0.5){
                output(i,0) =1;
            }
        }
        bool isCorrect = checkOutput(output, set[tp]->target);
        if(isCorrect){
            for(int i=0; i<nClass; i++){
                if(set[tp]->target(i,0) == 1){
                    correctResults[i] ++;
                    break;
                }
            }
        }
    }

    for(int i=0; i<nClass; i++){
        correctResults[i] = correctResults[i]/totalSample[i] * 100.0;
    }

    return correctResults;
}

double NeuralNetwork::getSetMSE(vector<DataEntry *> &set){
    int size = (int)set.size();
    double mse = 0;
    for ( int tp = 0; tp < size; tp++) {
        mat input = set[tp]->pattern;
        mse += (getVisualizeOutput(input)(0,0) - set[tp]->target(0, 0)) *
               (getVisualizeOutput(input)(0,0) - set[tp]->target(0, 0));

    }
    return mse/size;
}

void NeuralNetwork::updateWeights(vector<mat> deltaWeights, vector<mat> deltaBiass, double learningRate) {

    for(int i= 0; i < nLayer -1; i++){
        weights[i]  += learningRate * deltaWeights[i];

        if(deltaBiass[i].n_rows != 0){
            biass[i] += learningRate * deltaBiass[i];
        }
    }
}

mat NeuralNetwork::getOutput() {
    return layers[nLayer -1]->neurals;
}

mat NeuralNetwork::getVisualizeOutput(mat input) {

    mat tmpNeurals = input;

    for(int i=1; i < nLayer; i++){
        mat tmp = weights[i-1] * tmpNeurals;
        if(biass[i - 1].n_cols != 0){
            tmp = tmp+ biass[i -1];
        }

        tmpNeurals = activationFunction(tmp, layers[i]->activeFunc);
    }

    return tmpNeurals;

}

vector<string> NeuralNetwork::getNeuralInfoForVisualize() {
    vector<string> output;
    output.push_back("Network infomation: ");
    if(netType == CLASSIFICATION){
        output.push_back("Network type: classification");
    }
    else{
        output.push_back("Network type: regression");
    }
    output.push_back("nlayer: " +to_string(nLayer) );
    for(int i =0; i < nLayer; i++){
        string layerInfo = "Layer " + to_string(i+1) + " has " + to_string(layers[i]->nNeurals) + " neurals; ";
        layerInfo += "activation function: " + activeFuncNameToString(layers[i]->activeFunc);
        output.push_back(layerInfo);
    }
    return output;
}

vector<Layer *> NeuralNetwork::getLayers(){
    return layers;
}


bool NeuralNetwork::saveWeights(string filename) {
    fstream outputFile;
    outputFile.open(filename, ios::out);

    if ( outputFile.is_open() ) {

        outputFile<<nLayer<<"\n";
        outputFile<<netType<<"\n";

        for(int i=0;i<nLayer; i++){
            outputFile<<layers[i]->nNeurals<<"\t"<<layers[i]->isBias <<"\t"<<layers[i]->activeFunc <<"\n";
        }

        for(int i=0; i < nLayer -1; i++){
            int currLayerNeural = layers[i]->nNeurals;
            int nextLayerNeural = layers[i+1]->nNeurals;

            for(int r=0; r < nextLayerNeural; r++){
                for(int c = 0; c< currLayerNeural; c++){
                    outputFile<<weights[i](r,c)<<"\t";
                }
                outputFile<<"\n";
            }
        }

        for(int i=0; i < nLayer -1; i++){
            if(layers[i]->isBias){
                for(int b =0; b< layers[i+1]->nNeurals; b++){
                    outputFile<<biass[i](b,0)<<"\t";
                }
                outputFile<<"\n";
            }
        }

        //print success
        cout << endl << "Neuron weights saved to '" << filename << "'" << endl;

        //close file
        outputFile.close();

        return true;
    }
    else {
        cout << endl << "Error - Weight output file '" << filename << "' could not be created: " << endl;
        return false;
    }

}
