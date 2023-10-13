#include <iostream>
#include <string>

#include "circuit.h"

using namespace std;

// global variables
Circuit currentCircuit;

void readNewNetlist()
{
    cout << "\nSelect one of the following options:" << endl << endl;

    cout << "A. Read netlist from relative path in input/" << endl;
    cout << "B. Read netlist from absolute path" << endl << endl;

    char option;
    cin >> option;
    switch(option)
    {
        case 'A': 
        {
            cout << "\nEnter netlist file path: ";
            string netlist;
            cin >> netlist;
            currentCircuit = Circuit("../input/" + netlist);
            break;
        }
        case 'B': 
        {
            cout << "\nEnter netlist file path: ";
            string netlist;
            cin >> netlist;
            currentCircuit = Circuit(netlist);
            break;
        }
    }
}

void computeCurrent()
{

}

void computeVoltage()
{

}

void displayMenu()
{
    cout << "\n=====================================================\n";
    cout << "\nSelect one of the following options:" << endl << endl;

    cout << "A. Read a new netlist" << endl;
    cout << "B. Compute current values for the current netlist" << endl;
    cout << "C. Compute voltage values for the current netlist" << endl;
    cout << "D. Exit" << endl << endl;

    cout << "Current netlist: " << endl;
    currentCircuit.print();

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