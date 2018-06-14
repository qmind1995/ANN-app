#ifndef LAYER_H
#define LAYER_H

#include "../utils.cpp"

class Trainer;
class NeuralNetwork;
class BatchTrainer;

using namespace arma;
using namespace parameters;

class Layer
{
public:
    Layer(int nNeurals, bool isBias, int activeFunc = parameters::NONE);
    mat getErrGradient(mat error);
    int nNeurals;
    bool isBias;
    void setNeuralsValue(mat values);
    void activation();
    int activeFunc;
    friend NeuralNetwork;
    friend Trainer;
    friend BatchTrainer;
private:
    mat neurals;
};

#endif // LAYER_H
