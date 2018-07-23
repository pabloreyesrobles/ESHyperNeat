#ifndef _NEURALNETWORK_H
#define _NEURALNETWORK_H

#include <vector>
#include <math.h>

#define SIGMOID_CONST 	4.9
#define GAUSS_DESV		0.1

using namespace std;

namespace ANN_USM
{

    enum NeuronType
    {
        NONE = 0,
        N_INPUT,
        N_HIDDEN,
        N_OUTPUT,
        BIAS
    };

    enum ActivationFunction
    {
        SIGNED_SIGMOID = 0,   // Sigmoid function   (default) (blurred cutting plane)
        UNSIGNED_SIGMOID,
        TANH,
        SIGNED_STEP,          // Treshold (0 or 1)  (cutting plane)
        UNSIGNED_STEP,
        SIGNED_GAUSS,         // Gaussian           (symettry)
        UNSIGNED_GAUSS,
        ABS,                  // Absolute value |x| (another symettry)
        SIGNED_SINE,          // Sine wave          (smooth repetition)
        UNSIGNED_SINE,
        LINEAR,               // Linear f(x)=x      (combining coordinate frames only)
    };

    class Connection
    {
    public:
    	unsigned int source_id;
    	unsigned int target_id;
    	double weight;
    	double signal;

    	//double hebb_rate;
    	//double hebb_pre_rate;

    	bool operator==(Connection const& other) const
    	{
    		return ((this->source_id == other.source_id) && (this->target_id && other.target_id));
    	}
    };

    class Neuron
    {
    public:
    	//unsigned int id;
    	vector <double> coordinates;				// 2D defined neuron. Could be defined by a vector of doubles
    	double a_input; 						 	// Sum of all input connection
    	double a_output;							// Sum of all input connection but activated as output of the neuron

    	NeuronType type;							
    	ActivationFunction activation_function;

    	void Activate();

    	bool operator==(Neuron const& other) const
    	{
    		return ((this->coordinates[0] == other.coordinates[0]) && (this->coordinates[1] && other.coordinates[1]) && (this->type && other.type));
    	}
    };

    class NeuralNetwork
    {
    public:
    	vector<Connection> connections;
    	vector<Neuron> neurons;
    	unsigned int num_inputs, num_outputs;

    	NeuralNetwork();
    	//NeuralNetwork(GeneticEncoding genome);

    	void ActivateNet();

    	//void Adapt(); 								// Hebbian learning
    	void ResetValues();								// Reset all activation input and output on neurons
    	void Clear();									// Clear connection and neurons vector

    	void Input(vector<double> input);
    	vector <double> Output();

    	void AddNeuron(Neuron n_neuron);
    	void AddConnection(Connection n_connection);

    	Connection GetConnection(unsigned int id_connection);
    	Neuron GetNeuron(unsigned int id_neuron);
    };
}

#endif //_NEURALNETWORK_H
