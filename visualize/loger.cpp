#include "loger.h"

Loger::Loger(QTextEdit *logTE) :logTextEdit(logTE) {

}

void Loger::writeNetworkLog(vector<Layer*> layers){
    logTextEdit->clear();
    logTextEdit->setAlignment(Qt::AlignCenter);
    logTextEdit->setFont(QFont ("Courier", 13));
    logTextEdit->insertPlainText("=======Neural Network Infomation======= \n");
    logTextEdit->setAlignment(Qt::AlignLeft);
    logTextEdit->insertPlainText("N layers: "+ QString::number(layers.size())+" \n");
    for(int i =0; i<layers.size(); i++){
        logTextEdit->insertPlainText("Layer "+QString::number(i+1)+" has "
                                         + QString::number(layers[i]->nNeurals) + " neurals");
        if(layers[i]->isBias){
            logTextEdit->insertPlainText(" and bias");
        }
        string activeFuncName = activeFuncNameToString(layers[i]->activeFunc);
        logTextEdit->insertPlainText(". Activation function: " + QString::fromStdString(activeFuncName));
        logTextEdit->insertPlainText("\n");
    }
    logAtEpoch = 0; // new network without training.
}

void Loger::writeTrainInfo(){
    logTextEdit->setAlignment(Qt::AlignCenter);
    logTextEdit->insertPlainText("\n=======Training Info======= \n");
    logTextEdit->setAlignment(Qt::AlignLeft);
    logTextEdit->insertPlainText("Training strategy: ");
    if(strategy == SINGLESAMPLE){
        logTextEdit->insertPlainText("single sample \n");

    }
    else{
        logTextEdit->insertPlainText("batch \n");
    }

    logTextEdit->insertPlainText("\n");
}

void Loger::writeTrainingLog(){
    if(trainer->trainingSetMSE.size() != trainer->validationSetMSE.size() ||
       trainer->trainingSetAccuracy.size() != trainer->validationSetAccuracy.size()){
        return;
    }

    int crrTrainEpoch = trainer->trainingSetMSE.size();
    if(netType == CLASSIFICATION){
        crrTrainEpoch = trainer->trainingSetAccuracy.size();
    }

    if(logAtEpoch < crrTrainEpoch){
        for(int i = logAtEpoch; i< crrTrainEpoch; i++){
            if(netType == REGRESSTION){
                logTextEdit->insertPlainText("Epoch "+ QString::number(i+1)+ ": training set MSE = "
                                                 + QString::number(trainer->trainingSetMSE[i]) + "\n");
                logTextEdit->insertPlainText("\t"+ QString::number(i+1)+ ": validation set MSE = "
                                                 + QString::number(trainer->validationSetMSE[i]) + "\n");
            }
            else{
                logTextEdit->insertPlainText("Epoch "+ QString::number(i+1)+ ": training set accuracy = "
                                                 + QString::number(trainer->trainingSetAccuracy[i]) + "\n");
                logTextEdit->insertPlainText("\t"+ QString::number(i+1)+ ": validation set accuracy = "
                                                 + QString::number(trainer->validationSetAccuracy[i]) + "\n");
            }

        }
        logAtEpoch = crrTrainEpoch;
    }

}

void Loger::setInfo(Trainer *_trainer, int _strategy, int _netType){
    trainer = _trainer;
    strategy = _strategy;
    netType = _netType;
}
