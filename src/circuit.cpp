#include "circuit.h"

#include <fstream>
#include <sstream>

using namespace std;

// default constructor
Circuit::Circuit() {}

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
    makeGoodMatrices();

    // printNodeVoltages();
    // printSourceCurrents();
    // printBatteries();

    for(int i = 0; i < this->sourceCurrents.size(); i++)
        currents.insert({"V" + to_string(i + 1), sourceCurrents[i]});

    for(int i = 0; i < resistorCurrents.size(); i++) 
        currents.insert({"R" + to_string(i + 1), resistorCurrents[i]});

    
}

// utlitiy for print debugging
void Circuit::printNodeVoltages()
{
    // print node voltages
    cout << "Voltage values at nodes: " << endl;
    for(int i = 0; i < this->nodeVoltages.size(); i++) {
        cout << setw(12) << nodeVoltages[i] << endl;
    }
    cout << endl;
}

// utlitiy for print debugging
void Circuit::printSourceCurrents()
{
    cout << "\n\nCurrent values through sources: " << endl;
    for(int i = 0; i < this->sourceCurrents.size(); i++) {
        cout << setw(12) << sourceCurrents[i] << endl;
    }
    cout << endl;
}

// utlitiy for print debugging
void Circuit::printBatteries()
{
    cout << "Voltage Sources (source | destination | voltage):" << endl;
    for (tuple<int, int, double> &tuple : this->batteries)
    {
        cout << setw(3)
             << get<0>(tuple) << setw(3)
             << get<1>(tuple) << setw(3)
             << get<2>(tuple)
             << endl;
    }
    cout << endl;
}

// utlitiy for print debugging
void Circuit::printResistors()
{
    cout << "Resistors (source | destination | resistance):" << endl;
    for (tuple<int, int, double> &tuple : this->resistors)
    {
        cout << setw(3)
             << get<0>(tuple) << setw(3)
             << get<1>(tuple) << setw(3)
             << get<2>(tuple)
             << endl;
    }
    cout << endl;
}

// utlitiy for print debugging
void Circuit::printBranchIncidenceMatrix()
{
    cout << "Branch Incidence Matrix:" << endl;
    for (vector<double> row : this->branchIncidenceMatrix)
    {
        for (int element : row)
        {
            cout << setw(3) << element;
        }
        cout << endl;
    }

    cout << endl;
}

void Circuit::constructBranchIncidenceMatrix()
{
    // Get number of branches
    int numBranches = this->batteries.size() + this->resistors.size();

    // Get number of nodes
    int numNodes = 0;
    for (const tuple<int, int, double> &tuple : this->batteries)
    {
        if (get<0>(tuple) > numNodes)
            numNodes = get<0>(tuple);
        if (get<1>(tuple) > numNodes)
            numNodes = get<1>(tuple);
    }

    for (const tuple<int, int, double> &tuple : this->resistors)
    {
        if (get<0>(tuple) > numNodes)
            numNodes = get<0>(tuple);
        if (get<1>(tuple) > numNodes)
            numNodes = get<1>(tuple);
    }
    numNodes += 1;

    // Initialize branch incidence matrix
    this->branchIncidenceMatrix.resize(numNodes, vector<double>(numBranches, 0));

    // Populate branch incidence matrix
    int branchIndex = 0;
    for (tuple<int, int, double> tuple : this->batteries)
    {
        branchIncidenceMatrix[get<0>(tuple)][branchIndex] = 1.0;
        branchIncidenceMatrix[get<1>(tuple)][branchIndex] = -1.0;
        branchIndex++;
    }

    for (tuple<int, int, double> tuple : this->resistors)
    {
        branchIncidenceMatrix[get<0>(tuple)][branchIndex] = 1;
        branchIncidenceMatrix[get<1>(tuple)][branchIndex] = -1;
        branchIndex++;
    }
    // Remove the first row from the matrix to get reduced matrix
    // if (!this->branchIncidenceMatrix.empty()) {
    //     this->branchIncidenceMatrix.erase(this->branchIncidenceMatrix.begin());
    // }
}

void printMatrix(vector<vector<double>> matrix)
{
    for (vector<double> row : matrix)
    {
        for (double element : row)
        {
            cout << setw(12) << element;
        }
        cout << endl;
    }
    cout << endl;
}

void Circuit::makeGoodMatrices()
{
    int numNodes = branchIncidenceMatrix.size() + batteries.size(); // Include supernodes
    vector<vector<double>> G(numNodes, vector<double>(numNodes, 0.0));

    // Construct the conductance matrix for resistors
    for (auto &resistor : resistors)
    {
        int i = get<0>(resistor);
        int j = get<1>(resistor);
        double R = get<2>(resistor);
        double conductance = 1.0 / R;

        G[i][i] += conductance;
        G[j][j] += conductance;
        G[i][j] -= conductance;
        G[j][i] -= conductance;
    }

    // Initialize current source vector
    vector<double> Is(numNodes, 0.0);

    // Construct conductance matrix for voltage sources and update current source vector
    int supernode = branchIncidenceMatrix.size(); // Initial supernode index
    for (auto &battery : batteries)
    {
        int i = get<0>(battery);    // Source node
        int j = get<1>(battery);    // Destination node
        double V = get<2>(battery); // Voltage value

        G[i][supernode] = 1;
        G[supernode][i] = 1;
        G[j][supernode] = -1;
        G[supernode][j] = -1;

        Is[supernode] = V;

        supernode++; // Increment supernode index for next voltage source
    }

    // Remove first column and first row (corresponding to the reference node)
    G.erase(G.begin());
    for (vector<double> &row : G)
    {
        row.erase(row.begin());
    }
    Is.erase(Is.begin());

    G = getMatrixWithNewColumn(&G, Is);

    // printMatrix(G);

    // Solve
    vector<double> V = this->solveMatrix(&G, 0);

    // Add 0 to the beginning of the solution vector
    V.insert(V.begin(), 0);

    // Print solution
    // for (double element : V)
    // {
    //     cout << setw(12) << element;
    // }

    // Add to node voltages all values - the number of voltage sources
    // this->nodeVoltages.resize(V.size()-batteries.size());
    for (long long unsigned int i = 0; i < V.size()-batteries.size(); i++)
    {
        this->nodeVoltages.push_back(V[i]);
    }

    // Add to source currents all values - the number of voltage sources
    for (long long unsigned int i = V.size()-batteries.size(); i < V.size(); i++)
    {
        this->sourceCurrents.push_back(V[i]);
    }

    resistorCurrents = this->getResistorCurrents();
}


void transpose(vector<vector<int>> matrix)
{
    for (long long unsigned int i = 0; i < matrix.size(); i++)
    {
        for (long long unsigned int j = 0; j < matrix[0].size(); j++)
        {
            int temp = matrix[i][j];
            matrix[i][j] = matrix[j][i];
            matrix[j][i] = temp;
        }
    }
}

void multiply(vector<vector<int>> matrix, vector<vector<int>> matrix2)
{
    vector<vector<int>> result;
    result.resize(matrix.size(), vector<int>(matrix2[0].size(), 0));

    for (long long unsigned int i = 0; i < matrix.size(); i++)
    {
        for (long long unsigned int j = 0; j < matrix2[0].size(); j++)
        {
            for (long long unsigned int k = 0; k < matrix[0].size(); k++)
            {
                result[i][j] += matrix[i][k] * matrix2[k][j];
            }
        }
    }
}

void ensureZerosAtBottom(vector<vector<double>> *matrix_ptr, int depth)
{
    for (long long unsigned int i = depth; i < matrix_ptr->size(); i++)
    {
        if ((*matrix_ptr)[i][depth] == 0)
        {
            for (long long unsigned int j = i + 1; j < matrix_ptr->size(); j++)
            {
                if ((*matrix_ptr)[j][depth] != 0)
                {
                    swap((*matrix_ptr)[i], (*matrix_ptr)[j]);
                    break;
                }
            }
        }
    }
}

vector<double> computeFactors(vector<vector<double>> *matrix_ptr, int depth)
{
    vector<double> factors;
    for (long long unsigned int i = depth + 1; i < matrix_ptr->size(); i++)
    {
        if ((*matrix_ptr)[depth][depth] != 0)
        {
            factors.push_back((*matrix_ptr)[i][depth] / (*matrix_ptr)[depth][depth]);
        }
        else
        {
            // Handle potential division by zero, for instance by throwing an exception
        }
    }
    return factors;
}

void forwardElimination(vector<vector<double>> *matrix_ptr, vector<double> factors, int depth)
{
    int currentRowIndex = depth + 1;
    long long unsigned int factorIndex = 0;
    while (factorIndex < factors.size())
    {
        // Multiply the row at 'depth' by the current factor and store in new vector
        vector<double> diffRow;
        for (long long unsigned int j = depth; j < (*matrix_ptr)[0].size(); j++)
        {
            diffRow.push_back((*matrix_ptr)[depth][j] * factors[factorIndex]);
        }

        // Subtract the new vector from the current row
        for (long long unsigned int i = depth; i < (*matrix_ptr)[currentRowIndex].size(); i++)
        {
            (*matrix_ptr)[currentRowIndex][i] -= diffRow[i - depth];
        }

        currentRowIndex++;
        factorIndex++;
    }
}

vector<double> Circuit::solveMatrix(vector<vector<double>> *matrix_ptr, int depth = 0)
{
    // printMatrix(*matrix_ptr);
    ensureZerosAtBottom(matrix_ptr, depth);
    // printMatrix(*matrix_ptr);

    vector<double> factors = computeFactors(matrix_ptr, depth);
    forwardElimination(matrix_ptr, factors, depth);
    // printMatrix(*matrix_ptr);

    if ((matrix_ptr->size() - depth) > 1)
    {
        solveMatrix(matrix_ptr, depth + 1);
    }

    double cost = 0;
    for (long long unsigned int i = depth + 1; i < (*matrix_ptr)[0].size() - 1; i++)
    {
        cost += (*matrix_ptr)[depth][i] * (*matrix_ptr)[i][(*matrix_ptr)[0].size() - 1];
        (*matrix_ptr)[depth][i] = 0;
    }
    (*matrix_ptr)[depth][(*matrix_ptr)[0].size() - 1] -= cost;
    (*matrix_ptr)[depth][(*matrix_ptr)[0].size() - 1] /= (*matrix_ptr)[depth][depth];
    (*matrix_ptr)[depth][depth] = 1;

    // printMatrix(*matrix_ptr);
    // Return last column as a vector
    vector<double> result;
    for (long long unsigned int i = 0; i < matrix_ptr->size(); i++)
    {
        result.push_back((*matrix_ptr)[i][matrix_ptr->size()]);
    }
    return result;
}

void Circuit::addBattery(istringstream &in)
{
    string name, source, destination, voltage;

    getline(in, name, ' ');
    getline(in, source, ' ');
    getline(in, destination, ' ');
    getline(in, voltage, ' ');

    tuple<int, int, double> battery = make_tuple(stoi(source), stoi(destination), stod(voltage));
    this->batteries.push_back(battery);
}

void Circuit::addResistor(istringstream &in)
{
    string name, source, destination, resistance;

    getline(in, name, ' ');
    getline(in, source, ' ');
    getline(in, destination, ' ');
    getline(in, resistance, ' ');

    tuple<int, int, double> resistor = make_tuple(stoi(source), stoi(destination), stod(resistance));
    this->resistors.push_back(resistor);
}

// add new column to existing matrix
vector<vector<double>> Circuit::getMatrixWithNewColumn(vector<vector<double>> *matrix_ptr, const vector<double> &newColumn) const
{
    // Check if the new column has the same number of rows as the existing matrix
    if (newColumn.size() != matrix_ptr->size())
    {
        // Handle error: Number of rows in the new column must match the existing matrix
        return *matrix_ptr; // Return the original matrix if there is an error
    }

    // Create a new matrix by copying the original matrix
    vector<vector<double>> newMatrix = *matrix_ptr;

    // Add the new column to the new matrix
    for (long long unsigned int i = 0; i < newMatrix.size(); ++i)
    {
        newMatrix[i].push_back(newColumn[i]);
    }

    // Return the new matrix with the added column
    return newMatrix;
}

vector<vector<double>> Circuit::getMatrixWithNewColumn(const vector<double> &newColumn) const
{
    // Check if the new column has the same number of rows as the existing matrix
    if (newColumn.size() != branchIncidenceMatrix.size())
    {
        // Handle error: Number of rows in the new column must match the existing matrix
        return branchIncidenceMatrix; // Return the original matrix if there is an error
    }

    // Create a new matrix by copying the original matrix
    vector<vector<double>> newMatrix = branchIncidenceMatrix;

    // Add the new column to the new matrix
    for (long long unsigned int i = 0; i < newMatrix.size(); ++i)
    {
        newMatrix[i].push_back(newColumn[i]);
    }

    // Return the new matrix with the added column
    return newMatrix;
}

vector<double> Circuit::getCurrentVector()
{
    int nrows = this->branchIncidenceMatrix.size();
    vector<double> zero_vector(nrows, 0.0);
    vector<vector<double>> incidence_mat_with_0 = Circuit::getMatrixWithNewColumn(zero_vector);
    // print current vlaues

    // vector<double> current_vector = solveMatrix(&incidence_mat_with_0);
    return vector<double>();
}

vector<double> Circuit::getVoltageDrop()
{
    vector<double> currentVector = Circuit::getCurrentVector();
    vector<double> voltageDropVector(currentVector.size(), 0);
    int idx = 0;
    for (const tuple<int, int, double> &tuple : this->batteries)
    {
        voltageDropVector[idx] = double(get<2>(tuple));
        idx++;
    }
    for (const tuple<int, int, double> &tuple : this->resistors)
    {
        voltageDropVector[idx] = double(currentVector[idx] * (get<2>(tuple)));
        idx++;
    }
    return voltageDropVector;
}

vector<double> Circuit::getVotlageNodes()
{
    vector<double> voltageDropVec = Circuit::getVoltageDrop();
    vector<vector<double>> incidence_mat_with_voltage_drop = Circuit::getMatrixWithNewColumn(voltageDropVec);
    vector<double> nodesVector = solveMatrix(&incidence_mat_with_voltage_drop);
    return nodesVector;
}

vector<double> Circuit::getResistorCurrents(){

    vector<double> resistorCurrentVec(this->resistors.size(),0);
    int i = 0;
    for (const tuple<int, int, double> &tuple : this->resistors){
        int source_idx =(int)get<0>(tuple);
        int dest_idx =(int)get<1>(tuple);
        int R = get<2>(tuple);
        resistorCurrentVec[i] = (this->nodeVoltages[source_idx] - this->nodeVoltages[dest_idx])/R;
        // cout << "Current through resistor " << i << ": " << resistorCurrentVec[i] << " A" << endl;
        i++;
    }
    
    return resistorCurrentVec;
}
