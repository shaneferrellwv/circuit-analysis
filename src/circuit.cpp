#include "circuit.h"

#include <fstream>
#include <sstream>

using namespace std;

// default constructor
// Circuit::Circuit() {}

// Circuit constructor
Circuit::Circuit(string netList)
{
    ifstream netListFile(netList);

    string component;
    while (getline(netListFile, component))
    {
        istringstream in(component);
        if (component[0] == 'V')
            this->addBattery(in);
        else if (component[0] == 'R')
            this->addResistor(in);
    }
    constructBranchIncidenceMatrix();
}

void Circuit::print()
{
    cout << "Voltage Sources (source | destination | voltage):" << endl;
    for (tuple<int, int, int> tuple : this->batteries)
    {
        cout << get<0>(tuple) << " "
             << get<1>(tuple) << " "
             << get<2>(tuple)
             << endl;
    }
    cout << endl;
    cout << "Resistors (source | destination | resistance):" << endl;
    for (tuple<int, int, int> tuple : this->resistors)
    {
        cout << get<0>(tuple) << " "
             << get<1>(tuple) << " "
             << get<2>(tuple)
             << endl;
    }
    cout << endl;
}

void Circuit::printBranchIncidenceMatrix(){
    cout << "Branch Incidence Matrix:" << endl;
    for(vector<int> row : this->branchIncidenceMatrix){
        for(int element : row){
            cout << setw(3) << element;
        }
        cout << endl;
    }

    cout << endl;
}

void Circuit::constructBranchIncidenceMatrix(){
    // Get number of branches
    int numBranches = this->batteries.size() + this->resistors.size();

    // Get number of nodes
    int numNodes = 0;
    for (const tuple<int, int, int> tuple : this->batteries)
    {
        if (get<0>(tuple) > numNodes)
            numNodes = get<0>(tuple);
        if (get<1>(tuple) > numNodes)
            numNodes = get<1>(tuple);
    }

    for (const tuple<int, int, int> tuple : this->resistors)
    {
        if (get<0>(tuple) > numNodes)
            numNodes = get<0>(tuple);
        if (get<1>(tuple) > numNodes)
            numNodes = get<1>(tuple);
    }
    numNodes+=1;

    // Initialize branch incidence matrix
    this->branchIncidenceMatrix.resize(numNodes, vector<int>(numBranches, 0));

    // Populate branch incidence matrix
    int branchIndex = 0;
    for (tuple<int, int, int> tuple : this->batteries)
    {
        branchIncidenceMatrix[get<0>(tuple)][branchIndex] = 1;
        branchIncidenceMatrix[get<1>(tuple)][branchIndex] = -1;
        branchIndex++;
    }

    for (tuple<int, int, int> tuple : this->resistors)
    {
        branchIncidenceMatrix[get<0>(tuple)][branchIndex] = 1;
        branchIncidenceMatrix[get<1>(tuple)][branchIndex] = -1;
        branchIndex++;
    }

}

void transpose(vector<vector<int>> matrix){
    for(int i = 0; i < matrix.size(); i++){
        for(int j = 0; j < matrix[0].size(); j++){
            int temp = matrix[i][j];
            matrix[i][j] = matrix[j][i];
            matrix[j][i] = temp;
        }
    }
}

void multiply(vector<vector<int>> matrix, vector<vector<int>> matrix2){
    vector<vector<int>> result;
    result.resize(matrix.size(), vector<int>(matrix2[0].size(), 0));

    for(int i = 0; i < matrix.size(); i++){
        for(int j = 0; j < matrix2[0].size(); j++){
            for(int k = 0; k < matrix[0].size(); k++){
                result[i][j] += matrix[i][k] * matrix2[k][j];
            }
        }
    }
}

void ensureZerosAtBottom(vector<vector<double>>* matrix_ptr, int depth) {
    for(int i = depth; i < matrix_ptr->size(); i++) {
        if((*matrix_ptr)[i][depth] == 0) {
            for(int j = i + 1; j < matrix_ptr->size(); j++) {
                if((*matrix_ptr)[j][depth] != 0) {
                    swap((*matrix_ptr)[i], (*matrix_ptr)[j]);
                    break;
                }
            }
        }
    }
}

vector<double> computeFactors(vector<vector<double>>* matrix_ptr, int depth) {
    vector<double> factors;
    for(int i = depth + 1; i < matrix_ptr->size(); i++) {
        if((*matrix_ptr)[depth][depth] != 0) {
            factors.push_back((*matrix_ptr)[i][depth] / (*matrix_ptr)[depth][depth]);
        } else {
            // Handle potential division by zero, for instance by throwing an exception
        }
    }
    return factors;
}

void printMatrix(vector<vector<double>> matrix){
    for(vector<double> row : matrix){
        for(double element : row){
            cout << setw(6) << element;
        }
        cout << endl;
    }
    cout << endl;
}

void forwardElimination(vector<vector<double>>* matrix_ptr, vector<double> factors, int depth){
    int currentRowIndex = depth + 1;
    int factorIndex = 0;
    while(factorIndex < factors.size()){
        // Multiply the row at 'depth' by the current factor and store in new vector
        vector<double> diffRow;
        for(int j = depth; j < (*matrix_ptr)[0].size(); j++){
            diffRow.push_back((*matrix_ptr)[depth][j] * factors[factorIndex]);
        }

        // Subtract the new vector from the current row
        for(int i = depth; i < (*matrix_ptr)[currentRowIndex].size(); i++){
            (*matrix_ptr)[currentRowIndex][i] -= diffRow[i - depth];
        }

        currentRowIndex++;
        factorIndex++;
    }
}

vector<double> solveMatrix(vector<vector<double>>* matrix_ptr, int depth = 0){
    // printMatrix(*matrix_ptr);
    ensureZerosAtBottom(matrix_ptr, depth);
    // printMatrix(*matrix_ptr);

    vector<double> factors = computeFactors(matrix_ptr, depth);
    forwardElimination(matrix_ptr, factors, depth);
    // printMatrix(*matrix_ptr);

    if((matrix_ptr->size() - depth) > 1){
        solveMatrix(matrix_ptr, depth + 1);
    }

    double cost = 0;
    for(int i = depth+1; i < (*matrix_ptr)[0].size()-1; i++){
        cost += (*matrix_ptr)[depth][i] * (*matrix_ptr)[i][(*matrix_ptr)[0].size()-1];
        (*matrix_ptr)[depth][i] = 0;
    }
    (*matrix_ptr)[depth][(*matrix_ptr)[0].size()-1] -= cost;
    (*matrix_ptr)[depth][(*matrix_ptr)[0].size()-1] /= (*matrix_ptr)[depth][depth];
    (*matrix_ptr)[depth][depth] = 1;

    // printMatrix(*matrix_ptr);
    // Return last column as a vector
    vector<double> result;
    for(int i = 0; i < matrix_ptr->size(); i++){
        result.push_back((*matrix_ptr)[i][matrix_ptr->size()]);
    }
    return result;
}

void Circuit::addBattery(istringstream& in)
{
    int source, destination, voltage;
    string name;
    in >> name;
    in >> source;
    in >> destination;
    in >> voltage;

    tuple<int, int, int> battery = make_tuple(source, destination, voltage);
    this->batteries.push_back(battery);
}

void Circuit::addResistor(istringstream& in)
{
    string name;
    int source, destination, resistance;
    in >> name;
    in >> source;
    in >> destination;
    in >> resistance;

    tuple resistor = make_tuple(source, destination, resistance);
    this->resistors.push_back(resistor);
}