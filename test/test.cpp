#include <vector>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <ESHYPERNEAT>

#define INPUTS 3
#define OUTPUTS 1

using namespace std;
using namespace ANN_USM;

vector<double *> inputVector;
vector<double *> outputVector;

Population *cppn_neat;

ESHyperNeat eshyperneat;

int main()
{
	srand (time(0));
	
	inputVector.reserve(INPUTS);
	outputVector.reserve(OUTPUTS);

	for (unsigned int i; i < INPUTS, i++){
		*inputVector[i] = 0.0;
	}

	for (unsigned int i; i < OUTPUTS, i++){
		*outputVector[i] = 0.0;
	}

	*cppn_neat = new Population(argv[2], argv[3], (char*)"NEAT", (char*)"./NEAT_organisms/");

	eshyperneat = ESHyperNeat(inputVector, outputVector, (char *)"eshyperneat_config.json");

	vector <vector <double>> fitnesses;
	double fitness;

	for(int i = 0; i < cppn_neat->GENERATIONS; i++)
	{
		for (int j = 0; j < cppn_neat->POPULATION_MAX; j++)
		{
			// Improve this
			if(!eshyperneat.createSubstrateConnections(&cppn_neat->organisms[j]))
				continue;
			clog << "---------------------------------------------" << endl;
			clog << "Generation: " << i << ", Population: " << j << endl;
			fitness = evaluate_xor();
			fitnesses[i].pushback(fitness);
			if (fitness > 15.0){
				break;
			}
		}
		cppn_neat->epoch();
	}

	eshyperneat.getHyperNeatOutputFunctions((char*)"genetic_encoding");

	eshyperneat.printConnectionFile((char*)"genetic_encoding");

	
	return 0;
}

double evaluate_xor(){

	double error = 0;
	vector <double> net_output;

	eshyperneat.substrate->Flush();

	*inputVector[0] = 1;
	*inputVector[1] = 0;
	*inputVector[2] = 1;

	eshyperneat.substrate->UpdateInputs();
	eshyperneat.evaluateSubstrateConnections();
	net_output = eshyperneat.substrate->GetOutputs();
	error += abs(1 - net_output[0]);

	clog << "Input: 1 0 - Output: " << net_output[0] << endl;
	
	*inputVector[0] = 0;
	*inputVector[1] = 1;
	*inputVector[2] = 1;

	eshyperneat.substrate->UpdateInputs();
	eshyperneat.evaluateSubstrateConnections();
	net_output = eshyperneat.substrate->GetOutputs();
	error += abs(1 - net_output[0]);

	clog << "Input: 0 1 - Output: " << net_output[0] << endl;

	*inputVector[0] = 0;
	*inputVector[1] = 0;
	*inputVector[2] = 1;

	eshyperneat.substrate->UpdateInputs();
	eshyperneat.evaluateSubstrateConnections();
	net_output = eshyperneat.substrate->GetOutputs();
	error += abs(0 - net_output[0]);

	clog << "Input: 0 0 - Output: " << net_output[0] << endl;

	*inputVector[0] = 1;
	*inputVector[1] = 1;
	*inputVector[2] = 1;

	eshyperneat.substrate->UpdateInputs();
	eshyperneat.evaluateSubstrateConnections();
	net_output = eshyperneat.substrate->GetOutputs();
	error += abs(0 - net_output[0]);

	clog << "Input: 1 1 - Output: " << net_output[0] << endl;

	return pow(4 - error, 2);
}