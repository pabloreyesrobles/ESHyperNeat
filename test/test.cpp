#include <vector>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <ESHYPERNEAT>

#define INPUTS 2
#define OUTPUTS 1

using namespace std;
using namespace ANN_USM;

vector <double> inputVector;
vector <double> outputVector;

Population *cppn_neat;

ESHyperNeat *eshyperneat;

NeuralNetwork net;

double evaluate_xor();
double fitness_neat(Genetic_Encoding);

int main(int argc, char *argv[])
{
	clog << "Initializing..." << endl;
	srand (time(0));
		
	for (unsigned int i = 0; i < INPUTS; i++){
		inputVector.push_back(0.0);
	}

	for (unsigned int i = 0; i < OUTPUTS; i++){
		outputVector.push_back(0.0);
	}

	clog << "Initializing ANN" << endl;
	cppn_neat = new Population((char*)"user_def", (char*)"genetic_encoding", (char*)"NEAT", (char*)"./NEAT_organisms/");

	cppn_neat->survivalSelection = true;
	cppn_neat->allowClones = true;

	cppn_neat->survivalThreshold = 0.3;
	cppn_neat->eliteOffspringParam = 0.02;

	clog << "Initializing ESHYPERNEAT" << endl;
	eshyperneat = new ESHyperNeat(inputVector, outputVector, (char *)"eshyperneat_config.json");

	net = NeuralNetwork();

	//vector <vector <double>> fitnesses;
	//vector <double> t_fitnesses;
	double fitness;
	double max_fitness = 0;

	clog << "Initialized!" << endl;
	/*
	if(!eshyperneat->createSubstrateConnections(&(cppn_neat->organisms[0])));

	clog << "Substrate_created!" << endl;
	
	fitness = evaluate_xor();
	clog << "-----------------------------------------------" << endl;
	clog << "Fitnesses: " << fitness << endl;
	*/
	for(int i = 0; i < cppn_neat->GENERATIONS; i++)
	{
		//t_fitnesses.clear();
		clog << "Generation: " << i << endl;
		for (int j = 0; j < cppn_neat->POPULATION_MAX; j++)
		{
			// Improve this
			
			if(!eshyperneat->createSubstrateConnections(&cppn_neat->organisms.at(j), net)) continue;
			
			fitness = evaluate_xor();

			//if (cppn_neat->organisms[j].fitness > 9 || fitness > 9) clog << cppn_neat->organisms[j].fitness << " - " << fitness << endl;
			
			/*
			if (fitness > max_fitness){
				clog << "Generation: " << i << " Population: " << j;
				clog << " fitness: " << fitness << endl;
				max_fitness = fitness;
			}
			*/
			
			cppn_neat->organisms[j].fitness = fitness;
			//cppn_neat->organisms[j].fitness = fitness_neat(cppn_neat->organisms[j]);

			//t_fitnesses.push_back(fitness);
			//clog << "Fitnesses_" << i << j << ": " << fitness;
			if (fitness > 15.0){
				//clog << " REACHED MAX" << endl;
			}
					
		}
		//clog << endl;
		//fitnesses.push_back(t_fitnesses);
		cppn_neat->Epoch();
	}
	//clog << "max_fitness: " << max_fitness << endl;
	//*/

	//eshyperneat->getHyperNeatOutputFunctions((char*)"genetic_encoding");

	//eshyperneat->printConnectionFile((char*)"genetic_encoding");

	
	return 0;
}

double evaluate_xor(){

	double error = 0;
	vector <double> net_output;
	vector <double> t_input;
	//t_input.push_back(0.0);
	t_input.push_back(0.0);
	t_input.push_back(0.0);

	net.ResetValues();

	t_input[0] = 1;
	t_input[1] = 0;
	//t_input[2] = 1;

	net.Input(t_input);
	for (unsigned int i = 0; i < MaxDepth+1; i++) net.ActivateNet();
	net_output = net.Output();
	error += abs(1 - abs(net_output[0]));	
	
	t_input[0] = 0;
	t_input[1] = 1;
	//t_input[2] = 1;

	net.Input(t_input);
	for (unsigned int i = 0; i < MaxDepth+1; i++) net.ActivateNet();
	net_output = net.Output();
	error += abs(1 - abs(net_output[0]));
	t_input[0] = 0;
	t_input[1] = 0;
	//t_input[2] = 1;

	net.Input(t_input);
	for (unsigned int i = 0; i < MaxDepth+1; i++) net.ActivateNet();
	net_output = net.Output();
	error += abs(0 - abs(net_output[0]));

	t_input[0] = 1;
	t_input[1] = 1;
	//t_input[2] = 1;

	net.Input(t_input);
	for (unsigned int i = 0; i < MaxDepth+1; i++) net.ActivateNet();
	net_output = net.Output();
	error += abs(0 - abs(net_output[0]));
	
	return pow(4 - error, 2);
}


double fitness_neat(Genetic_Encoding organism){

	vector<double> output;
	vector<double> input;

	double error_sum = 0;
	//cout << organism << endl;

	// XOR(0,0) -> 0
	input.push_back(0);
	input.push_back(0);

	output = organism.eval(input);

	error_sum += abs(output.at(0));
	input.clear();

	// XOR(0,1) -> 1
	input.push_back(0);
	input.push_back(1);
	output = organism.eval(input);
	error_sum += abs(1 - output.at(0));
	input.clear();

	// XOR(1,0) -> 1
	input.push_back(1);
	input.push_back(0);
	output = organism.eval(input);
	error_sum += abs(1 - output.at(0));
	input.clear();

	// XOR(1,1) -> 0
	input.push_back(1);
	input.push_back(1);
	output = organism.eval(input);
	error_sum += abs(output.at(0));
	input.clear();

	return pow(4 - error_sum, 2);
}