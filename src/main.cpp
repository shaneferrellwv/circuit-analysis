#include <iostream>
#include <string>

#include "circuit.h"

using namespace std;

// global variables
Circuit currentCircuit;
string currentNetlist;

void readNewNetlist()
{
    cout << "\nSelect one of the following options:" << endl << endl;

    cout << "A. Read netlist from relative path in input/" << endl;
    cout << "B. Read netlist from absolute path" << endl << endl;

    char option;
    cin >> option;
    
    cout << "\nEnter netlist file path: ";
    string netlist;
    cin >> netlist;

    switch(option)
    {
        case 'A': 
        {
            currentCircuit = Circuit("../input/" + netlist);
            break;
        }
        case 'B': 
        {
            currentCircuit = Circuit(netlist);
            break;
        }
    }
    
    // check if netlist is valid

    // if netlist invalid, print erorr message and halt

    // if netlist valid
    // check if circuit is valid (NaN does not appear)

    // if circuit invalid (NaN appears in computations), print error message and halt

    // if circuit valid
    // set currentNetlist file
}



void computeCurrent()
{
    vector<double> currents = currentCircuit.getCurrentVector();


    cout << "\nSelect one of the following options:" << endl << endl;

    cout << "A. Compute currents across all branches in circuit" << endl;
    cout << "B. Compute currents across specified branches" << endl << endl;

    
    char option;
    cin >> option;

    switch(option)
    {
        case 'A': 
        {
            
            break;
        }
        case 'B': 
        {
            
            break;
        }
    }
}

void computeVoltage()
{
    cout << "\nSelect one of the following options:" << endl << endl;

    cout << "A. Compute voltages of all nodes in circuit" << endl;
    cout << "B. Compute voltage drops between all nodes in circuit" << endl;
    cout << "C. Compute voltages and voltage drops from a list of nodes/pairs of nodes" << endl << endl;
}

void displayMenu()
{
    cout << "\n=====================================================\n\n";

    cout << "Current netlist: " << currentNetlist << endl << endl;

    cout << "Select one of the following options:" << endl << endl;

    cout << "A. Read a new netlist" << endl;
    cout << "B. Compute current values for the current netlist" << endl;
    cout << "C. Compute voltage values for the current netlist" << endl;
    cout << "D. Exit" << endl << endl;
    // currentCircuit.printBatteries();
    // currentCircuit.printResistors();
    // currentCircuit.printBranchIncidenceMatrix();
}

int main()
{
    char option;
    bool stop = false;
    while (!stop)
    {
        displayMenu();

        cin >> option;
        switch(option)
        {
            case 'A': {
                readNewNetlist();
                break;
            }
            case 'B': {
                computeCurrent();
                break;
            }
            case 'C': {
                computeVoltage();
                break;
            }
            case 'D': {
                stop = true;
                break;
            }
        }
    }
}