#ifndef DATAREADER_H
#define DATAREADER_H

#include "dataentry.h"
#include <armadillo>
#include "QFile"
#include "QTextStream"

class TrainingDataSet {
public:

    std::vector<DataEntry*> trainingSet;

    std::vector<DataEntry*> validationSet;

    TrainingDataSet(){}
    TrainingDataSet(vector<DataEntry*> data, int percentile);

    void clear() {
        trainingSet.clear();
        validationSet.clear();
    }
};


class DataReader
{
public:
    DataReader();
    ~DataReader();
    std::vector<DataEntry*> data;

    void read_Mnist(string filename, vector<arma::mat> &vec, int max_number_of_images);
    void read_Mnist_Label(string filename, vector<double> &vec, int max_number_of_images);
    void read_Mnist_Label_QVer(QFile *labelFile, QFile*imageFile ,int numdata);
    void read_Input(string imgFileName, string labelFileName, int number_of_images);
    void read_Mnist_HOG(string filename, vector<arma::mat> &vec, int max_number_of_images);
    void read_RegDataByQFile(QFile *inputFile, QFile *outputFile, int numdata);
private:
    int ReverseInt (int i);
};

#endif // DATAREADER_H
