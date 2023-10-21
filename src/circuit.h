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

    vector<vector<int>> branchIncidenceMatrix;

    // constructors
    Circuit(){};
    Circuit(string netList);

    // public methods
    void print();
    void printBranchIncidenceMatrix();

    // private methods
private:
    void addBattery(istringstream& in);
    void addResistor(istringstream& in);
    void constructBranchIncidenceMatrix();
};