# Important Functions
In this document, we will give a brief overview of how important methods work. For the purpose of clarity, some syntax might be omitted 

## Constructor Circuit()
The input file is read and parsed, adding voltage sources (we will call them batteries) and resistors to their appropriate vectors.

Next, rather than implementing a branch incidence matrix, we create a conductance matrix. We found this made it easier to compute the desired values. 

Finally, the currents for the voltage sources and resistors are combined to a currents vector and the voltages at each node are added to a voltages vector

## makeConductanceMatrices()
First, we create a new matrix G out of a vector of vectors of doubles. Then the conductances are computed as the reciprical of the resistances and added into the appropriate spots given the source and destination of each resistor. 

Next, we add a 1 and -1 in the source and destination nodes for every supernode and vector named Is corresponding to the solution is created. 

Then the first row and column, corresponding to the ground, are removed as they are not needed. 

Finally, the solution of the matrix computation G * Is = 0 is computed. The solution vector contains both voltages and currents so those values are then put into their appropriate vectors for easier access

## getCurrentFromPoints()
This is the function that takes in a list of nodes denoting a path between two nodes and computes the current between them. Essentially, it functions by computing the total resistence along a path, considering both parallel and series resistors. Then the total voltage drop along the path is computed pairwise between nodes. Then the current that is returned is simply the total voltage divided by the total resistence. 