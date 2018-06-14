#ifndef LOGER_H
#define LOGER_H

#include "qtextedit.h"
#include "ann/layer.h"
#include "ann/trainer.h"
#include "../utils.cpp"

using namespace std;

class Loger
{
public:
    Loger(QTextEdit * logTE);

    void writeNetworkLog(vector<Layer *> layers);
    void writeTrainingLog();
    void setInfo(Trainer *_trainer, int _strategy, int _netType);
    void writeTrainInfo();
private:
    QTextEdit * logTextEdit;
    Trainer * trainer;
    int strategy;
    int netType;
    int logAtEpoch;
};

#endif // LOGER_H
