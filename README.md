# circuit-analysis

This tool is used to simulate simple electrical circuits and analyze voltages and currents of a circuit with voltage sources and resistors.

# Installation and Starting

Below are the steps to run the ``circuit-analysis`` tool:

1. Clone this repository to your computer.
2. In a terminal, navigate to the ``src/`` directory.
3. Run the command ``mingw32-make`` to compile the code into an executable called ``circuit-analysis.exe``.
4. Run the executable with the command ``./circuit-analysis``.

# Important Usage Notes
1. To input a netlist for examination, place the netlist file into the current working directory and the user will be prompted to input the file name
2. To get the current or voltage between two nodes, say node_1 and node_n, the user must input the list of nodes as the following [$node_1,node_2$],[$node_1,node_3$],...,[$node_{n-1},node_n$] where $node_i$ and $node_{i+1}$ are connected
3. 

# Dependencies

To use the make tool, you must first ensure that MinGW's mingw32-make package is installed on your device and MinGW's ``bin`` directory is in your system's PATH.

# Assumptions
1. The circuit (and netlist) only has voltage sources and resistors
2. The 0th node is ground
3. All netlist files have lines have the form $Vi$ $i_s$ $i_d$ $d_m$ or  $Ri$ $i_s$ $i_d$ $d_m$ where:
* $Vi / Ri$ is a captial letter followed by a number denoting the ith node
* $i_s$ is an integer denoting the source node.
* $i_d$ is an integer denoting the destination node
* $d_m$ is a double denoting the magnitude of the component


# Error Handling
1. Checks if input netlist file ends with ".net". If not, throws an error and goes to main menu to restart
2. Checks if the input netlist file exists. If not, throws an error and goes to main menu to restart
3. A file is considered invalid if it does not follow the format listed in the assumption section. If this is the case, the program throws an error and goes to the main menu to restart
4. In particular, if a component has no magnitude (i.e. a voltage source has no voltage or resistor has no resistance), the program throws an error and goes to main menu to restart

