#ifndef DATAENTRY_H
#define DATAENTRY_H
#include <iostream>
#include <vector>

#include <armadillo>
using namespace std;

class DataEntry
{
public:

    //public members
    arma::mat pattern;	//all the patterns
    arma::mat target;		//all the targets

public:

    //constructor

    DataEntry(arma::mat p, arma::mat t): pattern(p), target(t) {}

    ~DataEntry() {

    }

};
#endif // DATAENTRY_H
