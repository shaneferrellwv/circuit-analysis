#include <vector>
#include <tuple>
#include <string>
#include <iostream>
#include <iomanip>
#include <map>

using namespace std;

struct Circuit
{
    // member variables
    typedef vector<tuple<int, int, double>> tupleVector;
    tupleVector batteries;
    tupleVector resistors;

    // Node Voltages
    vector<double> nodeVoltages;

    // Source currents
    vector<double> sourceCurrents;
    vector<double> resistorCurrents;
    map<string, double> currents;
    map<string, double> voltages;
    map<string, double> nodeToNames;

    vector<vector<double>> branchIncidenceMatrix;

    // constructors
    Circuit();
    Circuit(string netList);

    // public methods
    void printBatteries();
    void printResistors();
    void printNodeVoltages();
    void printSourceCurrents();
    void printBranchIncidenceMatrix();
    vector<double> getCurrentVector();
    vector<vector<double>> getMatrixWithNewColumn(const vector<double>& newColumn) const;
    vector<vector<double>> getMatrixWithNewColumn(vector<vector<double>> *matrix_ptr, const vector<double> &newColumn) const;
    vector<double> getVoltageDrop();
    vector<double> getVotlageNodes();
    vector<double> getResistorCurrents();
    double getCurrentFromPoints(const std::vector<std::pair<int, int>>& nodePairs);
    bool checkNodeListValidity(vector<pair<int, int>> nodePairs);

    // private methods
private:
    void addBattery(istringstream& in);
    void addResistor(istringstream& in);
    vector<double> solveMatrix(vector<vector<double>> *matrix_ptr, int depth);
    void constructBranchIncidenceMatrix();
    void makeConductanceMatrices();
};