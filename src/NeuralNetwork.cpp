#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include "NeuralNetwork.hpp"

using namespace ANN_USM;

NeuralNetwork::NeuralNetwork()
{
	num_inputs = 0;
	num_outputs = 0;
	// Clear any possible data in neurons and connections 
}

void NeuralNetwork::ActivateNet()
{
	for (unsigned int i = 0; i < connections.size(); i++)
	{
		connections[i].signal = neurons[connections[i].source_id].a_output*connections.weight;
	}

	for (unsigned int i = 0; i < connections.size(); i++)
	{
		neurons[connections[i].target_id].a_input += connections[i].signal;
	}

	for (unsigned int i = 0; i < neurons.size(); i++)
	{
		neurons[i].Activate();
	}
}

void NeuralNetwork::ResetValues()
{
	for (unsigned int i = 0; i < neurons.size(); i++)
	{
		neurons[i].a_input = 0;
		neurons[i].a_output = 0;
	}

	for (unsigned int i = 0; i < connections.size(); i++)
	{
		connections[i].signal = 0;
	}
}

void NeuralNetwork::Clear()
{
	neurons.clear();
	connections.clear();
	num_outputs = num_inputs = 0;
}

void NeuralNetwork::Input(vector<double> input)
{
	unsigned int input_size = input.size();
	if (input_size > num_inputs)
	{
		input_size = num_inputs;
	}

	for (unsigned int i = 0; i < input_size; i++)
	{
		neurons[i].a_output = input[i];
	}
}

vector <double> NeuralNetwork::Output()
{
	vector <double> t_output;
	for (unsigned int i = 0; i < num_outputs; i++)
	{
		t_output.push_back(neurons[num_inputs + i].a_output);
	}
	return t_output;
}

void NeuralNetwork::AddNeuron(Neuron n_neuron)
{
	neurons.push_back(n_neuron);
}

void NeuralNetwork::AddConnection(Connection n_connection)
{
	connections.push_back(n_connection);
}

Connection NeuralNetwork::GetConnection(unsigned int id_connection)
{
	return connections[id_connection];
}

Neuron NeuralNetwork::GetNeuron(unsigned int id_neuron)
{
	return neurons[id_neuron];
}
