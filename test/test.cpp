#include <vector>
#include <stdlib.h>
#include <iostream>
#include <HYPERNEAT>

#define INPUTS 4
#define OUTPUTS 4
#define ITER 20

using namespace std;
using namespace ANN_USM;

int main()
{
	srand (time(0));
	
	vector< double * > inputVector;
	vector< double * > outputVector;

	for(int i = 0; i < INPUTS; i++)
	{
		double * input = new double((100.0 - rand()%201)/100.0);
		double * output = new double(0.0);
		inputVector.push_back(input);
		outputVector.push_back(output);
	}

	HyperNeat hyperneat = HyperNeat(inputVector, outputVector, (char*)"hyperneat_config.json");

	hyperneat.createSubstrateConnections((char*)"genetic_encoding");

	hyperneat.evaluateSubstrateConnections();

	for(int i = 0; i < ITER; i++)
	{
		hyperneat.evaluateSubstrateConnections();

		for(int j  = 0; j < INPUT; j++)
		{
			*inputVector.at(j) = *outputVector.at(j);
		}
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