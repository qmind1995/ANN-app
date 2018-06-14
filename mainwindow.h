#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QFileDialog>
#include <QObject>
#include <QDateTime>

#include "dataReader/datareader.h"
#include "dialog/createlayerdialog.h"
#include "ann/neuralnetwork.h"
#include "ann/trainer.h"
#include "ann/batchtrainer.h"
#include "graphLib/edge.h"
#include "graphLib/node.h"
#include "graphLib/qcustomplot.h"
#include "visualize/netgraphicsscene.h"
#include "dataReader/dataentry.h"
#include "utils.cpp"
#include "visualize/graphdrawer.h"
#include "visualize/loger.h"

#define N_THREAD   1

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void displayRes();

    void on_trainsetSld_valueChanged(int value);

    void on_visualBtn_clicked();

    void on_createLayerBtn_clicked();

    void on_createdNet();
    void on_startTrainingBtn_clicked();

    void on_classRBtn_clicked();

    void on_regRBtn_clicked();

    void on_openNetFileBtn_clicked();

private:
    Ui::MainWindow *ui;
    QTimer timer;
    CreateLayerDialog * cDialog;
    TrainingDataSet* trSet;
    NeuralNetwork * nNet;
    QGraphicsScene * scene;
    Trainer * trainer;
    GraphDrawer * dataWiget;
    GraphDrawer * mse_accWidget;
    GraphDrawer * resultWidget;
    Loger * loger;
    pthread_t threads[N_THREAD];
    void visualizeTrainingDuration();
    void drawClass();
};

#endif // MAINWINDOW_H
