#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFile"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    ui->setupUi(this);
    connect(&timer, SIGNAL(timeout()), this, SLOT(displayRes()));
    ui->sampleCB->addItem("sin() regression");
    dataWiget = new GraphDrawer(ui->dataWidget, 3);
    mse_accWidget = new GraphDrawer(ui->mse_accWidget, 2);
    resultWidget = new GraphDrawer(ui->reg_classWidget, 2);
    loger = new Loger(ui->logTextEdit);
}

MainWindow::~MainWindow(){
    delete ui;
    if(cDialog != NULL){
        delete cDialog;
    }
}

void MainWindow::displayRes(){
    if(nNet != NULL){
        bool isReg = ui->regRBtn->isChecked();
        if(isReg){
            if(!resultWidget->isDrawing){
                resultWidget->drawReg(nNet);
            }
            if(!mse_accWidget->isDrawing){
                mse_accWidget->drawMSE(trainer, ui->epochSB->value());
            }
        }
        else{
            if(!resultWidget->isDrawing){
                resultWidget->drawClass(nNet, trSet);
            }
            if(!mse_accWidget->isDrawing){
                mse_accWidget->drawAcc(trainer, ui->epochSB->value());
            }
        }
        loger->writeTrainingLog();
    }

    if(nNet->isTraining != true){
        timer.stop();

        QString saveWeightDirName = QFileDialog::getExistingDirectory(this, QObject::tr("Save Weights"), "");
        if(saveWeightDirName.compare("") != 0){
            QDateTime timenow;
            QString timestamp  = QString::number( timenow.toTime_t()) ;
            QString weightFileName = saveWeightDirName + "/weight-"+ timestamp+".txt";
            nNet->saveWeights(weightFileName.toStdString());
        }
    }
}

void MainWindow::on_trainsetSld_valueChanged(int value){
    ui->trainSetLb->setText("Training set percentile (" + QString::number(value) + "%)");
}

void MainWindow::on_visualBtn_clicked(){
    if(ui->regRBtn->isChecked()){
        QFile *outfile = new QFile(":/dataFiles/data/sinData.txt");
        QFile *infile = new QFile(":/dataFiles/data/sinInput.txt");

        DataReader* dataReader = new DataReader();
        dataReader->read_RegDataByQFile(infile, outfile, 30000);

        trSet = new TrainingDataSet(dataReader->data, ui->trainsetSld->value());
        dataWiget->visualizeTrainData_regression(trSet);
    }
    else{
        QFile *outfile = new QFile(":/dataFiles/data/train-labels.idx1-ubyte");
        QFile *infile = new QFile(":/dataFiles/data/train-images.idx3-ubyte");

        DataReader* dataReader = new DataReader();
        dataReader->read_Mnist_Label_QVer(outfile,infile, 30000);
        trSet = new TrainingDataSet(dataReader->data, ui->trainsetSld->value());
        dataWiget->visualizeTrainData_Classification(trSet);
    }
}

void MainWindow::visualizeTrainingDuration(){

    bool isReg = ui->regRBtn->isChecked();

    ui->reg_classWidget->clearGraphs();
    ui->mse_accWidget->clearGraphs();

    ui->reg_classWidget->addGraph();
    ui->reg_classWidget->addGraph();
    ui->reg_classWidget->legend->setVisible(true);
    ui->reg_classWidget->legend->setFont(QFont("Helvetica",9));
    if(isReg){
        ui->reg_classWidget->graph(0)->setName("Ground Truth");
        ui->reg_classWidget->graph(1)->setName("Regression");
    }
    else{
        resultWidget->configClassG();
    }

    ui->mse_accWidget->addGraph(); //trainset mse
    ui->mse_accWidget->addGraph(); //validset mse
    ui->mse_accWidget->legend->setVisible(true);
    ui->mse_accWidget->legend->setFont(QFont("Helvetica",9));
    if(isReg){
        ui->mse_accWidget->graph(0)->setName("Training Set MSE");
        ui->mse_accWidget->graph(1)->setName("Validation Set MSE");
    }
    else{
        ui->mse_accWidget->graph(0)->setName("Training Set Accuracy");
        ui->mse_accWidget->graph(1)->setName("Validation Set Accuracy");
    }

    int strategy = 0;
    if(ui->singleSampleRbtn->isChecked()){
        strategy = SINGLESAMPLE;
    }
    else{
        strategy = BATCH;
    }
    int netType = 0;
    if(ui->regRBtn->isChecked()){
        netType = REGRESSTION;
    }
    else{
        netType = CLASSIFICATION;
    }

    loger->setInfo(trainer,strategy, netType);
    loger->writeTrainInfo();
    timer.start(200);
}

void *runtrainingThread(void *params){
    struct paramHolder * ps = reinterpret_cast<struct paramHolder *>(params);
    ps->net->isTraining = true;
    ps->trainer->trainNetwork(ps->tset);
}

void MainWindow::on_createLayerBtn_clicked(){
    int nLayers = ui->nLayerSB->value();

    cDialog = new CreateLayerDialog(this);
    cDialog->show();
    cDialog->addWidget(nLayers);
    connect(cDialog,SIGNAL (accepted()),this,SLOT (on_createdNet()));
}

void MainWindow::on_createdNet(){
    vector<Layer*> layers = this->cDialog->layers;
    if(ui->regRBtn->isChecked()){
        nNet = new NeuralNetwork(layers, REGRESSTION);
    }
    else{
        nNet = new NeuralNetwork(layers, CLASSIFICATION);
    }

    NetGraphicsScene * netScene = new NetGraphicsScene(this);
    ui->NetGraphicsView->setScene(netScene);
    ui->NetGraphicsView->show();
    netScene->visualNet(layers);
    loger->writeNetworkLog(layers);
}

void MainWindow::on_startTrainingBtn_clicked(){
    double learningRate =  ui->learningRateSB->value(),
           lamda = ui->lamdaSB->value();
    int epoch = ui->epochSB->value();
    double desiredMSE =ui->desiredMSESB->value();

    if(ui->singleSampleRbtn->isChecked()){
        //single sample training
       trainer  = new Trainer(nNet, learningRate, epoch, desiredMSE);
    }
    else{
        //batch training
        trainer = new BatchTrainer(nNet,learningRate, epoch, desiredMSE, ui->batchSizeSpinBox->value());
    }

    struct paramHolder * params = new paramHolder();
    params->net = this->nNet;
    params->trainer = trainer;
    params->tset = trSet;

    int rc1 = pthread_create(&threads[0], NULL,runtrainingThread,(void*) params);
    if (rc1){
        cout << "\nError: cannot create training thread! " << rc1 << endl;
        exit(-1);
    }
    visualizeTrainingDuration();
}

void MainWindow::on_classRBtn_clicked(){
    ui->sampleCB->clear();
    QStringList sample;
    sample<<"Handwritten Digit Recognition";
    ui->sampleCB->addItems(sample);
}

void MainWindow::on_regRBtn_clicked(){
    ui->sampleCB->clear();
    QStringList sample;
    sample<<"Sin() Regression";
    ui->sampleCB->addItems(sample);
}

void MainWindow::on_openNetFileBtn_clicked(){
    QString netInfoFileName = QFileDialog::getOpenFileName(this, QObject::tr("Net Info"), "","");
    nNet = new NeuralNetwork(netInfoFileName.toStdString());
    NetGraphicsScene * netScene = new NetGraphicsScene(this);
    ui->NetGraphicsView->setScene(netScene);
    ui->NetGraphicsView->show();
    vector<Layer*> layers = nNet->getLayers();
    netScene->visualNet(layers);
    loger->writeNetworkLog(layers);
}
