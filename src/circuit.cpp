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