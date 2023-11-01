#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <cmath>

#include "circuit.h"

using namespace std;

// global variables
Circuit currentCircuit;
string currentNetlist;

// utility functions
bool endsWithDotNet(const std::string &str)
{
    if (str.size() < 4)
        return false;

    return str.compare(str.size() - 4, 4, ".net") == 0;
}

bool fileExists(const std::string &path)
{
    return filesystem::exists(path);
}

bool hasNan(const std::vector<double> &vec)
{
    for (const auto &val : vec)
    {
        if (std::isnan(val))
        {
            return true;
        }
    }
    return false;
}

bool checkNetlistValidity(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        return false; // Unable to open file
    }

    std::string line;
    while (getline(file, line))
    {
        if (line.empty())
            continue; // Skip empty lines

        if ((line[0] != 'V' && line[0] != 'R') ||
            (line.size() < 2 || !isdigit(line[1])))
        {
            return false; // Line doesn't start with 'V' or 'R' followed by a digit
        }

        std::istringstream iss(line.substr(2)); // Skip the first two characters
        double num1, num2, num3;
        if (!(iss >> num1 >> num2 >> num3))
        {
            return false; // Line doesn't match the format
        }
    }

    return true; // All lines match criteria
}

void readNewNetlist()
{
    cout << "\nSelect one of the following options:" << endl
         << endl;

    cout << "A. Read netlist from relative path in input/" << endl;
    cout << "B. Read netlist from absolute path" << endl
         << endl;

    char option;
    cin >> option;

    cout << "\nEnter netlist file path: ";
    string netlist;
    cin >> netlist;

    // check if netlist is valid
    if (!endsWithDotNet(netlist))
    {
        cout << "\nError: file must be of file type .net" << endl;
        return;
    }

    Circuit c;
    switch (option)
    {
    case 'A':
    {
        if (!fileExists("../input/" + netlist))
        {
            cout << "\nError: "
                 << "input/" + netlist << " not found" << endl;
            return;
        }
        else if (!checkNetlistValidity("../input/" + netlist))
        {
            cout << "\nError: Netlist file invalid" << endl;
            return;
        }
        c = Circuit("../input/" + netlist);
        break;
    }
    case 'B':
    {
        if (!fileExists(netlist))
        {
            cout << "\nError: " << netlist << " not found" << endl;
            return;
        }
        else if (!checkNetlistValidity(netlist))
        {
            cout << "\nError: Netlist file invalid" << endl;
            return;
        }
        c = Circuit(netlist);
        break;
    }
    }

    // if netlist valid
    // check if circuit is valid (NaN does not appear)
    if (hasNan(c.sourceCurrents) || hasNan(c.nodeVoltages) || hasNan(c.resistorCurrents))
    {
        cout << "Error: netlist is invalid" << endl;
    }

    // if circuit invalid (NaN appears in computations), print error message and halt

    // if circuit valid
    // set currentNetlist file
    currentCircuit = c;
    currentNetlist = netlist;
}

void computeCurrent()
{
    vector<double> currents = currentCircuit.getCurrentVector();

    cout << "\nSelect one of the following options:" << endl
         << endl;

    cout << "A. Compute currents across all branches in circuit" << endl;
    cout << "B. Compute currents across a net list" << endl
         << endl;

    char option;
    cin >> option;
    cout << endl;

    switch (option)
    {
    case 'A':
    {
        for (auto it = currentCircuit.currents.begin(); it != currentCircuit.currents.end(); ++it)
        {
            cout << "I(" << it->first << "): " << it->second << endl;
        }
        break;
    }
    case 'B':
    {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Please enter net name or list of net names expected format [1,2] or [1,2],[2,0]: ";

        std::string input;
        getline(cin, input);

        std::vector<std::pair<int, int>> nodePairs;

        std::stringstream ss(input);
        std::string segment;
        while (getline(ss, segment, ']'))
        {
            std::string cleaned;
            for (char c : segment) 
            {
                if (!isspace(c) && c != '[' && c != ']') 
                {
                    cleaned += c;
                }
            }
            segment = cleaned;

            int firstNode = stoi(segment.substr(0, segment.find(',')));
            int secondNode = stoi(segment.substr(segment.find(',') + 1));
            nodePairs.push_back({firstNode, secondNode});
            
            ss.ignore(1, ',');

        }

        double totalCurrent = currentCircuit.getCurrentFromPoints(nodePairs);

        cout << "Total current: " << totalCurrent << endl;
        break;
    }
    }
}

void computeVoltage()
{
    cout << "\nSelect one of the following options:" << endl
         << endl;

    cout << "A. Compute voltages of all nodes in circuit" << endl;
    cout << "B. Compute voltage drops between all nodes in circuit" << endl;
    cout << "C. Compute voltages and voltage drops from a list of nodes/pairs of nodes" << endl
         << endl;
}

void displayMenu()
{
    cout << "\n=====================================================\n\n";

    cout << "Current netlist: " << currentNetlist << endl
         << endl;

    cout << "Select one of the following options:" << endl
         << endl;

    cout << "A. Read a new netlist" << endl;
    cout << "B. Compute current values for the current netlist" << endl;
    cout << "C. Compute voltage values for the current netlist" << endl;
    cout << "D. Exit" << endl
         << endl;
    // currentCircuit.printBatteries();
    // currentCircuit.printResistors();
    // currentCircuit.printBranchIncidenceMatrix();
}

int main()
{
    currentNetlist = "no netlist selected";

    char option;
    bool stop = false;
    while (!stop)
    {
        displayMenu();

        cin >> option;
        switch (option)
        {
        case 'A':
        {
            readNewNetlist();
            break;
        }
        case 'B':
        {
            computeCurrent();
            break;
        }
        case 'C':
        {
            computeVoltage();
            break;
        }
        case 'D':
        {
            stop = true;
            break;
        }
        }
    }
}