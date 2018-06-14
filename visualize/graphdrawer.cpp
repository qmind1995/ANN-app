#include "graphdrawer.h"


GraphDrawer::GraphDrawer(QCustomPlot *dataWidget, int numGraphs): myWidget(dataWidget){
    for(int i=0; i< numGraphs; i++){
        myWidget->addGraph();
    }
    isDrawing = false;
}

void GraphDrawer::configClassG(){
    myWidget->clearGraphs();
    myWidget->clearItems();
    myWidget->legend->clearItems();

    QLinearGradient gradient(0, 0, 0, 400);
    gradient.setColorAt(0, QColor(90, 90, 90));
    gradient.setColorAt(0.38, QColor(105, 105, 105));
    gradient.setColorAt(1, QColor(70, 70, 70));
    myWidget->setBackground(QBrush(gradient));

    errorBar = new QCPBars(myWidget->xAxis, myWidget->yAxis);
    correctBar = new QCPBars(myWidget->xAxis, myWidget->yAxis);

    errorBar->setAntialiased(false);
    correctBar->setAntialiased(false);

    errorBar->setName("Error Rates");
    errorBar->setPen(QPen(QColor(111, 9, 176).lighter(170)));
    errorBar->setBrush(QColor(111, 9, 176));
    correctBar->setName("Correct Rates");
    correctBar->setPen(QPen(QColor(250, 170, 20).lighter(150)));
    correctBar->setBrush(QColor(250, 170, 20));

    myWidget->legend->setVisible(true);
    myWidget->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
    myWidget->legend->setBrush(QColor(255, 255, 255, 100));
    myWidget->legend->setBorderPen(Qt::NoPen);
    myWidget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    errorBar->moveAbove(correctBar);
}

void GraphDrawer::makeDataPlot(){
    // generate some data:
    QVector<double> x(1000), y(1000);
    double delta  = 2*PI/1000;
    int index = 0;

    for (double i=0; index< 1000; i+= delta){
        x[index] = i;
        y[index] = sin(i);
        index ++;
    }

    // create graph and assign data to it:
    myWidget->graph(0)->setData(x, y);
    // give the axes some labels:
    myWidget->xAxis->setLabel("x");
    myWidget->yAxis->setLabel("sin(x)");
    // set axes ranges, so we see all data:
    myWidget->xAxis->setRange(0.0, 2*PI);
    myWidget->yAxis->setRange(-1.0, 1.0);

    QPen pen;
    pen.setColor(QColor(255,0,0));
    pen.setWidth(1);
    myWidget->graph(0)->setPen(pen);
    myWidget->replot();
}

void GraphDrawer::visualizeTrainData_regression(TrainingDataSet* trSet){
    myWidget->legend->setVisible(true);
    myWidget->legend->setFont(QFont("Helvetica",9));

    //draw throw truth
    makeDataPlot();
    QCPScatterStyle blueDotStyle = QCPScatterStyle(QCPScatterStyle::ssCross,Qt::blue , 1);
    QCPScatterStyle yellowDotStyle = QCPScatterStyle(QCPScatterStyle::ssCross,Qt::yellow , 1);

    myWidget->graph(1)->setScatterStyle(blueDotStyle);
    myWidget->graph(1)->setLineStyle(QCPGraph::lsNone);

    QVector<double> trainX(trSet->trainingSet.size()), trainY(trSet->trainingSet.size());
    for(int i =0; i< trSet->trainingSet.size(); i++){
        trainX[i] = trSet->trainingSet[i]->pattern(0,0);
        trainY[i] = trSet->trainingSet[i]->target(0,0);
    }
    myWidget->graph(1)->setData(trainX, trainY);

    myWidget->graph(2)->setScatterStyle(yellowDotStyle);
    myWidget->graph(2)->setLineStyle(QCPGraph::lsNone);
    QVector<double> validX(trSet->validationSet.size()), validY(trSet->validationSet.size());
    for(int i =0; i< trSet->validationSet.size(); i++){
        validX[i] = trSet->validationSet[i]->pattern(0,0);
        validY[i] = trSet->validationSet[i]->target(0,0);
    }
    myWidget->graph(2)->setData(validX, validY);

    myWidget->graph(0)->setName("Ground Truth");
    myWidget->graph(1)->setName("Training Set");
    myWidget->graph(2)->setName("Validation Set");

    myWidget->replot();
    myWidget->update();
}

void GraphDrawer::visualizeTrainData_Classification(TrainingDataSet * trSet){
    myWidget->clearGraphs();
    myWidget->clearItems();
    myWidget->legend->clearItems();
    vector<int> trainSetVote = voteOuput(trSet->trainingSet);
    vector<int> validSetVote = voteOuput(trSet->validationSet);

    QLinearGradient gradient(0, 0, 0, 400);
    gradient.setColorAt(0, QColor(90, 90, 90));
    gradient.setColorAt(0.38, QColor(105, 105, 105));
    gradient.setColorAt(1, QColor(70, 70, 70));
    myWidget->setBackground(QBrush(gradient));

    // create empty bar chart objects:
    QCPBars * trainSetBar = new QCPBars(myWidget->xAxis, myWidget->yAxis);
    QCPBars * validSetBar = new QCPBars(myWidget->xAxis, myWidget->yAxis);

    trainSetBar->setAntialiased(false);
    validSetBar->setAntialiased(false);
    trainSetBar->setStackingGap(1);
    validSetBar->setStackingGap(1);

    // set names and colors:
    trainSetBar->setName("Training Set");
    trainSetBar->setPen(QPen(QColor(111, 9, 176).lighter(170)));
    trainSetBar->setBrush(QColor(111, 9, 176));
    validSetBar->setName("Validation Set");
    validSetBar->setPen(QPen(QColor(250, 170, 20).lighter(150)));
    validSetBar->setBrush(QColor(250, 170, 20));

    // stack bars on top of each other:
    validSetBar->moveAbove(trainSetBar);

    // prepare x axis with country labels:
    QVector<double> ticks;
    QVector<QString> labels;
    for(int i=0; i< trainSetVote.size(); i++){
        ticks<< i+1;
        labels<< "Class "+QString::number(i+1);
    }
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    myWidget->xAxis->setTicker(textTicker);
//    myWidget->xAxis->setTickLabelRotation(60);
    myWidget->xAxis->setSubTicks(false);
    myWidget->xAxis->setTickLength(0, trainSetVote.size() +1);
    myWidget->xAxis->setRange(0, trainSetVote.size() +1);
    myWidget->xAxis->setBasePen(QPen(Qt::white));
    myWidget->xAxis->setTickPen(QPen(Qt::white));
    myWidget->xAxis->grid()->setVisible(true);
    myWidget->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
    myWidget->xAxis->setTickLabelColor(Qt::white);
    myWidget->xAxis->setLabelColor(Qt::white);

    // prepare y axis:
    int rangeY = 0;
    for(int i=0; i<trainSetVote.size(); i++){
        if(rangeY < trainSetVote[i]+ validSetVote[i]){
            rangeY = trainSetVote[i]+ validSetVote[i];
        }
    }
    myWidget->yAxis->setRange(0, rangeY + 0.001 * rangeY);// padding 0.1%
    myWidget->yAxis->setPadding(5); // a bit more space to the left border
    myWidget->yAxis->setLabel("Amount Of Data In \n Training Set \& Validation Set");
    myWidget->yAxis->setBasePen(QPen(Qt::white));
    myWidget->yAxis->setTickPen(QPen(Qt::white));
    myWidget->yAxis->setSubTickPen(QPen(Qt::white));
    myWidget->yAxis->grid()->setSubGridVisible(true);
    myWidget->yAxis->setTickLabelColor(Qt::white);
    myWidget->yAxis->setLabelColor(Qt::white);
    myWidget->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
    myWidget->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));

    // Add data:
    QVector<double> trainSetDataVote, validSetDataVote;
    for(int i=0; i<trainSetVote.size(); i++){
        trainSetDataVote<<trainSetVote[i];
        validSetDataVote<<validSetVote[i];
    }
    trainSetBar->setData(ticks, trainSetDataVote);
    validSetBar->setData(ticks, validSetDataVote);

    // setup legend:

    myWidget->legend->setVisible(true);
    myWidget->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
    myWidget->legend->setBrush(QColor(255, 255, 255, 100));
    myWidget->legend->setBorderPen(Qt::NoPen);
    myWidget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    myWidget->replot();
    myWidget->update();
}

void GraphDrawer::drawMSE(Trainer * trainer , int maxEpoch){
    if(isDrawing){
        return;
    }
    isDrawing = true;

    if(trainer != NULL && trainer->trainingSetMSE.size() >0){
        vector<double> tmpMSE = trainer->trainingSetMSE;
        int trainMSESize = tmpMSE.size();

        QVector<double> trainX(trainMSESize), trainY(trainMSESize);
        double maxMSE = tmpMSE[0];
        for(int i= 0; i<trainMSESize; i++){
            trainX[i] = i;
            trainY[i] = tmpMSE[i];

            if(trainY[i]> maxMSE){
                maxMSE =trainY[i];
            }
        }
        QCPScatterStyle dotStyle = QCPScatterStyle(QCPScatterStyle::ssTriangle);
        myWidget->graph(0)->setData(trainX,trainY);
        myWidget->graph(0)->setScatterStyle(dotStyle);

        vector<double> tmpValidMSE = trainer->validationSetMSE;
        int validMSESize = tmpValidMSE.size();
        QVector<double> validX(validMSESize), validY(validMSESize);
        for(int i=0; i<validMSESize; i++){
            validX[i] = i;
            validY[i] = tmpValidMSE[i];

            if(validY[i] > maxMSE){
                maxMSE = validY[i];
            }
        }
        QPen pen;
        pen.setColor(Qt::red);
        QCPScatterStyle vdotStyle = QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::red);
        myWidget->graph(1)->setData(validX,validY);
        myWidget->graph(1)->setScatterStyle(vdotStyle);
        myWidget->graph(1)->setPen(pen);

        // give the axes some labels:
        myWidget->xAxis->setLabel("Epoch");
        myWidget->yAxis->setLabel("MSE");
        // set axes ranges, so we see all data:
        myWidget->xAxis->setRange(0, maxEpoch +1);
        myWidget->yAxis->setRange(0, maxMSE);
        myWidget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
        myWidget->replot();
        myWidget->update();
    }
    isDrawing = false;
}

void GraphDrawer::drawAcc(Trainer * trainer , int maxEpoch){
    if(isDrawing){
        return;
    }
    isDrawing = true;
    if(trainer != NULL && trainer->trainingSetAccuracy.size() >0){
        vector<double> tmpAcc = trainer->trainingSetAccuracy;
        int trainAccSize = tmpAcc.size();

        QVector<double> trainX(trainAccSize), trainY(trainAccSize);
        for(int i= 0; i<trainAccSize; i++){
            trainX[i] = i;
            trainY[i] = tmpAcc[i];
        }
        QCPScatterStyle dotStyle = QCPScatterStyle(QCPScatterStyle::ssTriangle);
        myWidget->graph(0)->setData(trainX,trainY);
        myWidget->graph(0)->setScatterStyle(dotStyle);

        vector<double> tmpValidAcc = trainer->validationSetAccuracy;
        int validAccSize = tmpValidAcc.size();
        QVector<double> validX(validAccSize), validY(validAccSize);
        for(int i=0; i<validAccSize; i++){
            validX[i] = i;
            validY[i] = tmpValidAcc[i];
        }
        QPen pen;
        pen.setColor(Qt::red);
        QCPScatterStyle vdotStyle = QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::red);
        myWidget->graph(1)->setData(validX,validY);
        myWidget->graph(1)->setScatterStyle(vdotStyle);
        myWidget->graph(1)->setPen(pen);

        // give the axes some labels:
        myWidget->xAxis->setLabel("Epoch");
        myWidget->yAxis->setLabel("Accuracy(%)");
        // set axes ranges, so we see all data:
        myWidget->xAxis->setRange(0, maxEpoch+1);
        myWidget->yAxis->setRange(0, 101);
        myWidget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

        myWidget->replot();
        myWidget->update();
    }
    isDrawing = false;
}

void GraphDrawer::drawReg(NeuralNetwork * nNet){
    if(isDrawing){
        return;
    }
    isDrawing = true;
    int nPoint = 1000;
    this->makeDataPlot();
    QVector<double> x(nPoint), y(nPoint);
    for(int i=0; i<nPoint; i++){
        x[i] = uniformRandom(0, 2*PI);
        mat input = mat(1,1);
        input(0,0) = x[i];
        mat output = nNet->getVisualizeOutput(input);
        y[i] = output(0,0);
    }
    QCPScatterStyle dotStyle = QCPScatterStyle(QCPScatterStyle::ssCircle,Qt::black , 2);
    myWidget->xAxis->setRange(0.0, 2*PI);
    myWidget->yAxis->setRange(-1.0, 1.0);
    myWidget->graph(1)->setData(x,y);
    myWidget->graph(1)->setScatterStyle(dotStyle);
    myWidget->graph(1)->setLineStyle(QCPGraph::lsNone);
    myWidget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    myWidget->replot();
    myWidget->update();
    isDrawing = false;
}

void GraphDrawer::drawClass(NeuralNetwork * nNet, TrainingDataSet * trSet){
    if(isDrawing){
        return;
    }
    isDrawing = true;

    vector<double> correctRates = nNet->getSetAccuracyForEachClass(trSet->validationSet);

    QVector<double> ticks;
    QVector<QString> labels;
    for(int i=0; i< correctRates.size(); i++){
        ticks<< i+1;
        labels<< "Class "+QString::number(i+1);
    }
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    myWidget->xAxis->setTicker(textTicker);
    myWidget->xAxis->setSubTicks(false);
    myWidget->xAxis->setTickLength(0, correctRates.size() +1);
    myWidget->xAxis->setRange(0, correctRates.size() +1);
    myWidget->xAxis->setBasePen(QPen(Qt::white));
    myWidget->xAxis->setTickPen(QPen(Qt::white));
    myWidget->xAxis->grid()->setVisible(true);
    myWidget->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
    myWidget->xAxis->setTickLabelColor(Qt::white);
    myWidget->xAxis->setLabelColor(Qt::white);


    myWidget->yAxis->setRange(0, 101);
    myWidget->yAxis->setPadding(5); // a bit more space to the left border
    myWidget->yAxis->setLabel("Amount Of Data In \n Training Set \& Validation Set");
    myWidget->yAxis->setBasePen(QPen(Qt::white));
    myWidget->yAxis->setTickPen(QPen(Qt::white));
    myWidget->yAxis->setSubTickPen(QPen(Qt::white));
    myWidget->yAxis->grid()->setSubGridVisible(true);
    myWidget->yAxis->setTickLabelColor(Qt::white);
    myWidget->yAxis->setLabelColor(Qt::white);
    myWidget->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
    myWidget->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));


    QVector<double> errRates, corRates;

    for(int i=0; i < correctRates.size(); i++){
        corRates<< correctRates[i];
        errRates<< 100 - correctRates[i];
    }
    errorBar->setData(ticks, errRates);
    correctBar->setData(ticks, corRates);


    myWidget->replot();
    myWidget->update();
    isDrawing = false;
}
