#include "trainer.h"

Trainer::Trainer( NeuralNetwork *nn )	:	NN(nn),
                                            epoch(0),
                                            learningRate(LEARNING_RATE),
                                            desiredAccuracy(DESIRED_ACCURACY),
                                            maxEpochs(MAX_EPOCHS),
                                            trainingSetAccuracy(0),
                                            validationSetAccuracy(0),
                                            trainingSetMSE(0),
                                            validationSetMSE(0) {

    //create delta lists
    int nLayers = nn->nLayer;
    for(int i=0; i< nLayers - 1; i++){
        int layerSize = nn->layers[i]->nNeurals;
        int nextLayerSize = nn->layers[i+1]->nNeurals;

        mat deltaW = mat(nextLayerSize, layerSize);
        deltaW.zeros();
        deltaWeights.push_back(deltaW);

        mat deltaB;
        if(nn->layers[i]->isBias){
            deltaB = mat(nextLayerSize, 1);
            deltaB.zeros();
        }

        // if this layer has no bias => push empty(size = [0x0]); and Output layer has no bias.
        deltaBiass.push_back(deltaB);
    }

    //create error gradient storage
    //------------------------------------------
    // maybe no need
    for(int i=1; i< nLayers; i++){
        mat eG = mat(nn->layers[i]->nNeurals, 1);
        eG.zeros();
        errorGradients.push_back(eG);
    }
}

Trainer::Trainer(NeuralNetwork *untrainedNetwork, double _learningRate, long _epoch, double _desireMSE): NN(untrainedNetwork),
                                                                                    epoch(0),
                                                                                    learningRate(_learningRate),
                                                                                    desiredAccuracy(DESIRED_ACCURACY),
                                                                                    desiredMSE(_desireMSE),
                                                                                    maxEpochs(_epoch) {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    //create delta lists
    int nLayers = untrainedNetwork->nLayer;
    for(int i=0; i< nLayers - 1; i++){
        int layerSize = untrainedNetwork->layers[i]->nNeurals;
        int nextLayerSize = untrainedNetwork->layers[i+1]->nNeurals;

        mat deltaW = mat(nextLayerSize, layerSize);
        deltaW.zeros();
        deltaWeights.push_back(deltaW);

        mat deltaB;
        if(untrainedNetwork->layers[i]->isBias){
            deltaB = mat(nextLayerSize, 1);
            deltaB.zeros();
        }

        // if this layer has no bias => push empty(size = [0x0]); and Output layer has no bias.
        deltaBiass.push_back(deltaB);
    }

    //create error gradient storage
    //------------------------------------------
    // maybe no need
    for(int i=1; i< nLayers; i++){
        mat eG = mat(untrainedNetwork->layers[i]->nNeurals, 1);
        eG.zeros();
        errorGradients.push_back(eG);
    }

}

void Trainer::backpropagate( mat desiredOutputs ){

    //modify deltas between layers
    int nLayers = NN->nLayer;

    mat err = desiredOutputs - NN->layers[nLayers-1]->neurals;

    for(int i=nLayers-1; i >=1; i--){

        mat errorGradient = NN->layers[i]->getErrGradient(err);
        mat deltaW = errorGradient * NN->layers[i-1]->neurals.t();
        deltaWeights[i-1] = deltaW;
        mat deltaB;

        if( NN->biass[i-1].n_rows != 0 ){
            deltaB = errorGradient;
        }
        deltaBiass[i-1] = deltaB;
        err = NN->weights[i-1] .t() * errorGradient;
    }

    updateWeights();
}

inline void Trainer::updateWeights() {
    NN->updateWeights(deltaWeights, deltaBiass, learningRate);
}

void Trainer::runTrainingEpoch( vector<DataEntry*> trainingSet ) {
    //incorrect patterns
    double incorrectPatterns = 0; // use in classification
    double mse = 0; //use in regression

    int size = (int)trainingSet.size();
    //for every training pattern
    for ( int tp = 0; tp < size; tp++) {

        //feed inputs through network and backpropagate errors
        NN->feedForwardPattern( trainingSet[tp]->pattern );
        backpropagate( trainingSet[tp]->target );

        switch (NN->netType){
            case CLASSIFICATION: {
                //pattern correct flag
                bool patternCorrect = true;
                //check all outputs from neural network against desired values
                //pattern incorrect if desired and output differ
                patternCorrect = checkOutput(NN->clampOutput(), trainingSet[tp]->target);
                //if pattern is incorrect add to incorrect count
                if (!patternCorrect) incorrectPatterns++;
                break;
            }
            case parameters::REGRESSTION: {
                //increase mean square error
                mse += (NN->getOutput()(0,0) - trainingSet[tp]->target(0, 0)) *
                       (NN->getOutput()(0,0) - trainingSet[tp]->target(0, 0));
                break;
            }
        }
    }//end for

    //update training accuracy || MSE
    switch (NN->netType){
        case CLASSIFICATION: {
            trainingSetAccuracy.push_back(100 - (incorrectPatterns / trainingSet.size() * 100) );
            break;
        }
        case REGRESSTION: {
            trainingSetMSE.push_back( mse / trainingSet.size());
            break;
        }
    }
}

bool Trainer::checkOutput(mat output, mat target){
    int size = output.n_rows;
    for(int i =0; i<size; i++){
        if(abs(output(i,0) - target(i,0) ) > 0.1){
            return false;
        }
    }
    return true;
}

void Trainer::trainNetwork( TrainingDataSet* tSet ) {

    //reset epoch and log counters
    epoch = 0;
    //train network using training dataset for training and generalization dataset for testing

    while (epoch < this->maxEpochs) { //add more condition!!!!

        if(epoch > 0){
            if(NN->netType == CLASSIFICATION ){
                if(desiredAccuracy < validationSetAccuracy.back()){
                    break;
                }
            }
            else if(NN->netType == REGRESSTION){
                if(desiredMSE > validationSetMSE.back()){
                    break;
                }
            }
        }

        //use training set to train network

        this->runTrainingEpoch( tSet->trainingSet );
        switch (NN->netType){
        case CLASSIFICATION: {
            validationSetAccuracy.push_back(NN->getSetAccuracy(tSet->validationSet));
            break;
        }
        case REGRESSTION:{
            validationSetMSE.push_back(NN->getSetMSE(tSet->validationSet));
            break;
        }
        }

        //once training set is complete increment epoch
        epoch++;

    }//end while
    NN->isTraining = false;
}

bool Trainer::isTraining(){
    return NN->isTraining;
}
