#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include "NeuralNetwork.hpp"

using namespace ANN_USM;


inline double af_sigmoid_unsigned(double a_x, double w_T)
{
    return 1.0 / (1.0 + exp(-w_T * a_x));
}

inline double af_sigmoid_signed(double a_x, double w_T)
{
    double f_output = af_sigmoid_unsigned(a_x, w_T);
    return (f_output - 0.5) * 2.0;
}

inline double af_tanh(double a_x)
{
    return tanh(a_x);
}

inline double af_step_signed(double a_x)
{
    double f_output;
    if (a_x > 0.0) // Modify to ant value in x axis
    {
        f_output = 1.0;
    }
    else
    {
        f_output = -1.0;
    }

    return f_output;
}

inline double af_step_unsigned(double a_x)
{
    if (a_x > 0.0)
    {
        return 1.0;
    }
    else
    {
        return 0.0;
    }
}

inline double af_gauss_signed(double a_x, double d_gauss)
{
    double f_output = (1 / (2.5066 * d_gauss)) * exp(-0.5 * (a_x*a_x) / d_gauss); // TODO: Need separate a, b per activation function
    return (f_output - 0.5) * 2.0;
}

inline double af_gauss_unsigned(double a_x, double d_gauss)
{
    return (1 / (2.5066 * d_gauss)) * exp(-0.5 * (a_x*a_x) / d_gauss);
}

inline double af_abs(double a_x)
{
    return (a_x < 0.0) ? -a_x : a_x;
}

inline double af_sine_signed(double a_x, double freq, double phase)
{
    return sin(a_x * freq + phase);
}

inline double af_sine_unsigned(double a_x, double freq, double phase)
{
    double f_output = sin(a_x * freq + phase);
    return (f_output + 1.0) / 2.0;
}

inline double af_linear(double a_x, double cut)
{
    return (a_x + cut);
}

void Neuron::Activate()
{
	double x = a_input;
    a_input = 0;

    double f_x = 0.0;
    switch (activation_function)
    {
    case SIGNED_SIGMOID:
        f_x = af_sigmoid_signed(x, SIGMOID_CONST);
        break;
    case UNSIGNED_SIGMOID:
        f_x = af_sigmoid_unsigned(x, SIGMOID_CONST);
        break;
    case TANH:
        f_x = af_tanh(x);
        break;
    case SIGNED_STEP:
        f_x = af_step_signed(x);
        break;
    case UNSIGNED_STEP:
        f_x = af_step_unsigned(x);
        break;
    case SIGNED_GAUSS:
        f_x = af_gauss_signed(x, GAUSS_DESV);
        break;
    case UNSIGNED_GAUSS:
        f_x = af_gauss_unsigned(x, GAUSS_DESV);
        break;
    case ABS:
        f_x = af_abs(x);
        break;
    case SIGNED_SINE:
        f_x = af_sine_signed(x, M_PI, 0);
        break;
    case UNSIGNED_SINE:
        f_x = af_sine_unsigned(x, M_PI, 0);
        break;
    case LINEAR:
        f_x = af_linear(x, 0); // add cut
        break;
    default:
        f_x = af_sigmoid_unsigned(x, SIGMOID_CONST);
        break;

    }
    a_output = f_x;
}

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
		connections[i].signal = neurons[connections[i].source_id].a_output * connections[i].weight;
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
