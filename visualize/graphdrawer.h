#ifndef VISUALIZEDATAWIDGET_H
#define VISUALIZEDATAWIDGET_H

#include "graphLib/qcustomplot.h"
#include "dataReader/datareader.h"
#include "ann/trainer.h"
#include "../utils.cpp"

class GraphDrawer
{
public:
    GraphDrawer(QCustomPlot * dataWidget, int numGraphs);
    bool isDrawing;

    void makeDataPlot();
    void visualizeTrainData_regression(TrainingDataSet *trSet);
    void visualizeTrainData_Classification(TrainingDataSet *trSet);
    void drawMSE(Trainer *trainer, int maxEpoch);
    void drawAcc(Trainer *trainer, int maxEpoch);
    void drawReg(NeuralNetwork *nNet);
    void drawClass(NeuralNetwork *nNet, TrainingDataSet *trSet);
    void configClassG();
private:
    QCustomPlot * myWidget;
    int crrEpoch;
    QCPBars * errorBar;
    QCPBars * correctBar;

};

#endif // VISUALIZEDATAWIDGET_H
