#include <vector>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <ESHYPERNEAT>

#define INPUTS 3
#define OUTPUTS 1

using namespace std;
using namespace ANN_USM;

vector <double *> inputVector;
vector <double *> outputVector;

Population *cppn_neat;

ESHyperNeat *eshyperneat;

double evaluate_xor();

int main(int argc, char *argv[])
{
	clog << "Initializing..." << endl;
	srand (time(0));
	
	clog << "Initializing vectors:" << endl;
	
	for (unsigned int i = 0; i < INPUTS; i++){
		inputVector.push_back(new double);
	}

	clog << "Input vectors initialized" << endl;

	for (unsigned int i = 0; i < OUTPUTS; i++){
		outputVector.push_back(new double);
	}

	clog << "Output vectors initialized" << endl;

	eshyperneat = new ESHyperNeat(inputVector, outputVector, (char *)"eshyperneat_config.json");
	clog << "ESHyperNeat initialized" << endl;

	cppn_neat = new Population((char*)"user_def", (char*)"genetic_encoding", (char*)"NEAT", (char*)"./NEAT_organisms/");
	clog << "cppn_neat initialized" << endl;

	

	vector <vector <double>> fitnesses;
	double fitness;

	clog << "Initialized!" << endl;

	for(int i = 0; i < cppn_neat->GENERATIONS; i++)
	{
		clog << "Running generation: " << i + 1 << " - ";
		for (int j = 0; j < cppn_neat->POPULATION_MAX; j++)
		{
			clog << "population: " << j + 1 << endl;
			// Improve this
			if(!eshyperneat->createSubstrateConnections(&(cppn_neat->organisms[j])))
				continue;
			clog << "---------------------------------------------" << endl;
			clog << "Generation: " << i << ", Population: " << j << endl;
			fitness = evaluate_xor();
			fitnesses[i].push_back(fitness);
			if (fitness > 15.0){
				break;
			}
		}
		cppn_neat->epoch();
	}

	eshyperneat->getHyperNeatOutputFunctions((char*)"genetic_encoding");

	eshyperneat->printConnectionFile((char*)"genetic_encoding");

	
	return 0;
}

double evaluate_xor(){

	double error = 0;
	vector <double> net_output;

	eshyperneat->substrate->Flush();

	*inputVector[0] = 1;
	*inputVector[1] = 0;
	*inputVector[2] = 1;

	eshyperneat->substrate->UpdateInputs();
	eshyperneat->evaluateSubstrateConnections();
	net_output = eshyperneat->substrate->GetOutputs();
	error += abs(1 - net_output[0]);

	clog << "Input: 1 0 - Output: " << net_output[0] << endl;
	
	*inputVector[0] = 0;
	*inputVector[1] = 1;
	*inputVector[2] = 1;

	eshyperneat->substrate->UpdateInputs();
	eshyperneat->evaluateSubstrateConnections();
	net_output = eshyperneat->substrate->GetOutputs();
	error += abs(1 - net_output[0]);

	clog << "Input: 0 1 - Output: " << net_output[0] << endl;

	*inputVector[0] = 0;
	*inputVector[1] = 0;
	*inputVector[2] = 1;

	eshyperneat->substrate->UpdateInputs();
	eshyperneat->evaluateSubstrateConnections();
	net_output = eshyperneat->substrate->GetOutputs();
	error += abs(0 - net_output[0]);

	clog << "Input: 0 0 - Output: " << net_output[0] << endl;

	*inputVector[0] = 1;
	*inputVector[1] = 1;
	*inputVector[2] = 1;

	eshyperneat->substrate->UpdateInputs();
	eshyperneat->evaluateSubstrateConnections();
	net_output = eshyperneat->substrate->GetOutputs();
	error += abs(0 - net_output[0]);

	clog << "Input: 1 1 - Output: " << net_output[0] << endl;

	return pow(4 - error, 2);
}