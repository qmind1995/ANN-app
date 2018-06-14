#include "datareader.h"


#include <iostream>
#include <fstream>
#include <math.h>

using namespace arma;
using namespace std;


int DataReader::ReverseInt (int i) {
    unsigned char ch1, ch2, ch3, ch4;
    ch1 = i & 255;
    ch2 = (i >> 8) & 255;
    ch3 = (i >> 16) & 255;
    ch4 = (i >> 24) & 255;

    return((int) ch1 << 24) + ((int)ch2 << 16) + ((int)ch3 << 8) + ch4;
}


DataReader::DataReader(){

}

DataReader::~DataReader(){
    data.clear();
}

void DataReader::read_Mnist_Label_QVer(QFile *labelFile, QFile *imageFile, int numdata){

    if(!labelFile->open(QIODevice::ReadOnly) || !imageFile->open(QIODevice::ReadOnly)){
        return;
    }
    //read labels
    vector<int> labelVec(numdata);
    int magic_number = 0;
    int number_of_images =0;
    labelFile->read((char*) &magic_number, sizeof(magic_number));
    magic_number = ReverseInt(magic_number);
    labelFile->read((char*) &number_of_images,sizeof(number_of_images));
    number_of_images = ReverseInt(number_of_images);
    for(int i = 0; i < numdata; ++i) {
        unsigned char temp = 0;
        labelFile->read((char*) &temp, sizeof(temp));
        labelVec[i]= (int)temp;
    }

    //read images
    int n_rows = 0;
    int n_cols = 0;
    vector<mat> imageVec(numdata);
    imageFile->read((char*) &magic_number, sizeof(magic_number));
    magic_number = ReverseInt(magic_number);
    imageFile->read((char*) &number_of_images,sizeof(number_of_images));
    number_of_images = ReverseInt(number_of_images);
    imageFile->read((char*) &n_rows, sizeof(n_rows));
    n_rows = ReverseInt(n_rows);
    imageFile->read((char*) &n_cols, sizeof(n_cols));
    n_cols = ReverseInt(n_cols);
    for(int i = 0; i < numdata; ++i) {
        arma::mat tp(n_rows*n_cols,1);

        for(int r = 0; r < n_rows; ++r) {
            for(int c = 0; c < n_cols; ++c) {
                unsigned char temp = 0;
                imageFile->read((char*) &temp, sizeof(temp));
                tp((r+1)*c, 0) = (double) temp /255;
            }
        }
        imageVec[i] = tp;
    }

    //restructure data:
    for(int i = 0; i < numdata; ++i) {
        arma::mat target = mat(10,1);
        target.zeros();
        for(int idx= 0; idx < 10; idx++){
            if(labelVec[i] == idx){
                target[idx] =1;
                break;
            }
        }
        DataEntry *d = new DataEntry(imageVec[i],target);
        data.push_back(d);
    }
}

void DataReader::read_RegDataByQFile(QFile* inputFile, QFile* outputFile, int numdata){
    if(!inputFile->open(QIODevice::ReadOnly) || !outputFile->open(QIODevice::ReadOnly)){
        return;
    }
    QTextStream inFStr(inputFile),
                outFStr(outputFile);
    int index = 1;
    while(index <= numdata){
        QString iLine = inFStr.readLine(),
                oLine = outFStr.readLine();
        bool inOk(false), outOk(false);
        double input = iLine.toDouble(&inOk);
        double output = oLine.toDouble(&outOk);
        mat iD = mat(1,1);
        iD(0,0) = input;
        mat oD = mat(1,1);
        oD(0,0) = output;
        DataEntry *d = new DataEntry(iD,oD);
        data.push_back(d);
        index++;
    }
}


TrainingDataSet::TrainingDataSet(vector<DataEntry *> data, int percentile){
    int trainingSetSize = data.size() * percentile /100;

    for(int i=0; i< data.size(); i++){
        if(i<trainingSetSize){
            trainingSet.push_back(data[i]);
        }
        else{
            validationSet.push_back(data[i]);
        }
    }
}
