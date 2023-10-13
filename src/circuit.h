#include <vector>
#include <tuple>
#include <string>

#include <iostream>

using namespace std;

struct Circuit
{
    // member variables
    typedef vector<tuple<int, int, int>> tupleVector;
    tupleVector batteries;
    tupleVector resistors;

    // constructors
    Circuit();
    Circuit(string netList);

    // public methods
    void print();

    // private methods
private:
    void addBattery(istringstream& in);
    void addResistor(istringstream& in);
};