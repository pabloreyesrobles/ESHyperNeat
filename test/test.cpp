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
		
	for (unsigned int i = 0; i < INPUTS; i++){
		inputVector.push_back(new double);
	}

	for (unsigned int i = 0; i < OUTPUTS; i++){
		outputVector.push_back(new double);
	}

	cppn_neat = new Population((char*)"user_def", (char*)"genetic_encoding", (char*)"NEAT", (char*)"./NEAT_organisms/");
	eshyperneat = new ESHyperNeat(inputVector, outputVector, (char *)"eshyperneat_config.json");

	vector <vector <double>> fitnesses;
	vector <double> t_fitnesses;
	double fitness;

	clog << "Initialized!" << endl;

	/*
	if(!eshyperneat->createSubstrateConnections(&(cppn_neat->organisms[0])));
	
	fitness = evaluate_xor();
	clog << "-----------------------------------------------" << endl;
	clog << "Fitnesses: " << fitness << endl;
	*/
	//*
	for(int i = 0; i < cppn_neat->GENERATIONS; i++)
	{
		t_fitnesses.clear();
		
		for (int j = 0; j < cppn_neat->POPULATION_MAX; j++)
		{
			clog << "-----------------------------------------------" << endl;
			//clog << "Generation: " << i << " Population: " << j << endl;
			// Improve this
			if(!eshyperneat->createSubstrateConnections(&cppn_neat->organisms.at(j))) continue;
			
			fitness = evaluate_xor();

			cppn_neat->organisms[j].fitness = fitness;
			t_fitnesses.push_back(fitness);
			clog << "Fitnesses_" << i << j << ": " << fitness << endl;
			if (fitness > 15.0){
				break;
			}
			
		}
		//clog << endl;
		//fitnesses.push_back(t_fitnesses);
		cppn_neat->epoch();
	}
	//*/

	//eshyperneat->getHyperNeatOutputFunctions((char*)"genetic_encoding");

	//eshyperneat->printConnectionFile((char*)"genetic_encoding");

	
	return 0;
}

double evaluate_xor(){

	double error = 0;
	vector <double> net_output;
	vector <double> t_input;
	t_input.push_back(0.0);
	t_input.push_back(0.0);
	t_input.push_back(0.0);

	eshyperneat->substrate->Flush();

	t_input[0] = 1;
	t_input[1] = 0;
	t_input[2] = 1;

	eshyperneat->substrate->UpdateInputs(t_input);
	eshyperneat->evaluateSubstrateConnections();
	net_output = eshyperneat->substrate->GetOutputs();
	error += abs(1 - abs(net_output[0]));	
	
	t_input[0] = 0;
	t_input[1] = 1;
	t_input[2] = 1;

	eshyperneat->substrate->UpdateInputs(t_input);
	eshyperneat->evaluateSubstrateConnections();
	net_output = eshyperneat->substrate->GetOutputs();
	error += abs(1 - abs(net_output[0]));
	t_input[0] = 0;
	t_input[1] = 0;
	t_input[2] = 1;

	eshyperneat->substrate->UpdateInputs(t_input);
	eshyperneat->evaluateSubstrateConnections();
	net_output = eshyperneat->substrate->GetOutputs();
	error += abs(0 - abs(net_output[0]));

	t_input[0] = 1;
	t_input[1] = 1;
	t_input[2] = 1;

	eshyperneat->substrate->UpdateInputs(t_input);
	eshyperneat->evaluateSubstrateConnections();
	net_output = eshyperneat->substrate->GetOutputs();
	error += abs(0 - abs(net_output[0]));
	
	return pow(4 - error, 2);
}