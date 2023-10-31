#include <vector>
#include <tuple>
#include <string>
#include <iostream>
#include <iomanip>

using namespace std;

struct Circuit
{
    // member variables
    typedef vector<tuple<int, int, int>> tupleVector;
    tupleVector batteries;
    tupleVector resistors;

    vector<vector<double>> branchIncidenceMatrix;

    // constructors
    Circuit(){};
    Circuit(string netList);

    // public methods
    void print();
    void printBranchIncidenceMatrix();
    vector<double> getCurrentVector();
    vector<vector<double>> Circuit::getMatrixWithNewColumn(const vector<double>& newColumn) const;
    vector<double> Circuit::getVoltageDrop();
    vector<double> Circuit::getVotlageNodes();

    // private methods
private:
    void addBattery(istringstream& in);
    void addResistor(istringstream& in);
    void constructBranchIncidenceMatrix();
};