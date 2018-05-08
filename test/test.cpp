#include <vector>
#include <stdlib.h>
#include <iostream>
#include <ESHYPERNEAT>

#define INPUTS 3
#define OUTPUTS 1
#define ITER 20

using namespace std;
using namespace ANN_USM;

int main()
{
	srand (time(0));
	
	vector<double *> inputVector;
	vector<double *> outputVector;

	inputVector.reserve(3);
	outputVector.reserve(1);

	*outputVector[0] = 0;

	Population *cppn_neat = new Population(argv[2], argv[3], (char*)"NEAT", (char*)"./NEAT_organisms/");

	Genetic_Encoding *organism = new Genetic_Encoding();
	organism->load((char *)"genetic_encoding");

	ESHyperNeat eshyperneat = ESHyperNeat(inputVector, outputVector, (char *)"eshyperneat_config.json");
	double error = 0;

	for(int i = 0; i < ITER; i++)
	{
		eshyperneat.createSubstrateConnections(organism);

		*inputVector[0] = 1;
		*inputVector[1] = 0;
		*inputVector[2] = 1;

		eshyperneat.evaluateSubstrateConnections();
		

		*inputVector[0] = 0;
		*inputVector[1] = 1;
		*inputVector[2] = 1;

		*inputVector[0] = 0;
		*inputVector[1] = 0;
		*inputVector[2] = 1;

		*inputVector[0] = 1;
		*inputVector[1] = 1;
		*inputVector[2] = 1;
	}

	clog << endl;

	for(int i = 0; i < (int)inputVector.size(); i++)
		clog << "INPUT " << i << ":\t" << *inputVector.at(i) << endl;

	clog << endl;

	for(int i = 0; i < (int)outputVector.size(); i++)
		clog << "OUTPUT " << i << ":\t" << *outputVector.at(i) << endl;

	clog << endl;

	hyperneat.getHyperNeatOutputFunctions((char*)"genetic_encoding");

	hyperneat.printConnectionFile((char*)"genetic_encoding");

	
	return 0;
}