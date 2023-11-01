#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <cmath>
#include <limits>
#include <cctype>
#include <sstream>
#include <string>
#include <algorithm>

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

bool isInExpectedFormat(const std::string& str) {
    char bracket1, bracket2, bracket3, bracket4, comma1, comma2, comma3;
    int num1, num2, num3, num4;
    
    std::istringstream iss(str);
    if (!(iss >> bracket1 >> num1 >> comma1 >> num2 >> bracket2 >> comma2 >> 
              bracket3 >> num3 >> comma3 >> num4 >> bracket4)) {
        return false;  // String doesn't match the format
    }

    return (bracket1 == '[' && bracket2 == ']' && bracket3 == '[' && bracket4 == ']' && 
            comma1 == ',' && comma2 == ',' && comma3 == ',');
}

bool checkNetlistValidity(const std::string &filename){
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
    
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    cout << "\nEnter netlist file path: ";
    string netlist;
    getline(cin, netlist);

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

void computeCurrent() {
    vector<double> currents = currentCircuit.getCurrentVector();

    cout << "\nSelect one of the following options:\n\n";
    cout << "A. Compute currents across all branches in circuit\n";
    cout << "B. Compute currents across a net list\n\n";

    char option;
    cin >> option;
    cout << endl;

    switch (option) {
    case 'A':
        // Assuming currentCircuit.currents is a std::map or similar associative container
        for (const auto& current : currentCircuit.currents) {
            cout << "I(" << current.first << "): " << current.second << endl;
        }
        break;
    case 'B':
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Please enter net name or list of net names expected format [1,2] or [1,2],[2,0]: ";

        string input;
        getline(cin, input);

        vector<pair<int, int>> nodePairs;
        stringstream ss(input);
        string segment;
        char expectedChar;

        while (getline(ss, segment, ']')) {
            if (segment.empty() || segment[0] != '[') {
                cout << "Error: Each pair must be enclosed in brackets.\n";
                return;
            }
            segment.erase(remove_if(segment.begin(), segment.end(), ::isspace), segment.end());
            segment.erase(remove(segment.begin(), segment.end(), '['), segment.end());

            if (count(segment.begin(), segment.end(), ',') != 1) {
                cout << "Error: Invalid format, exactly one comma expected in each pair.\n";
                return;
            }

            size_t commaPos = segment.find(',');
            string firstNodeStr = segment.substr(0, commaPos);
            string secondNodeStr = segment.substr(commaPos + 1);

            if (!all_of(firstNodeStr.begin(), firstNodeStr.end(), ::isdigit) ||
                !all_of(secondNodeStr.begin(), secondNodeStr.end(), ::isdigit)) {
                cout << "Error: Non-integer value found.\n";
                return;
            }

            try {
                int firstNode = stoi(firstNodeStr);
                int secondNode = stoi(secondNodeStr);
                nodePairs.push_back({firstNode, secondNode});
            } catch (const exception& e) {
                cout << "Error: Invalid number format.\n";
                return;
            }

            if (ss >> expectedChar && expectedChar != ',') {
                cout << "Error: Expected comma between pairs.\n";
                return;
            }
            // ss.ignore(1, ',');
        }

        double totalCurrent = currentCircuit.getCurrentFromPoints(nodePairs);
        cout << "Total current: " << totalCurrent << endl;
        break;
    }
}

void computeVoltage()
{
    cout << "\nSelect one of the following options:" << endl
         << endl;

    cout << "A. Compute voltages of all nodes in circuit" << endl;
    cout << "B. Compute voltage drops between all nodes in circuit" << endl;
    cout << "C. Compute voltages and voltage drops from a list of nodes/pairs of nodes" << endl << endl;

    char option;
    cin >> option;
    cout << endl;

    switch(option)
    {
        case 'A': 
        {
            for (auto it = currentCircuit.voltages.begin(); it != currentCircuit.voltages.end(); ++it)
            {
                cout << "V(" << it->first << "): " << it->second << endl;
            }
            break;
        }
        case 'B': 
        {
            
        }
    }
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