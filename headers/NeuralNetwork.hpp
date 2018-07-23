#ifndef _NEURALNETWORK_H
#define _NEURALNETWORK_H

#include <vector>
#include <math.h>

#define SIGMOID_CONST 	4.9
#define GAUSS_DESV		0.1

using namespace std;

namespace ANN_USM
{
    double af_sigmoid_unsigned(double a_x, double w_T)
    {
        return 1.0 / (1.0 + exp(-w_T * a_x));
    }

    double af_sigmoid_signed(double a_x, double w_T)
    {
        double f_output = af_sigmoid_unsigned(a_x, w_T);
        return (f_output - 0.5) * 2.0;
    }

    double af_tanh(double a_x, double slope)
    {
        return tanh(a_x * slope);
    }

    double af_step_signed(double a_x)
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

    double af_step_unsigned(double a_x)
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

    double af_gauss_signed(double a_x, double d_gauss)
    {
        double f_output = (1 / (2.5066 * d_gauss)) * exp(-0.5 * (x*x) / d_gauss); // TODO: Need separate a, b per activation function
        return (f_output - 0.5) * 2.0;
    }

    double af_gauss_unsigned(double a_x, double aSlope, double d_gauss)
    {
        return (1 / (2.5066 * d_gauss)) * exp(-0.5 * (x*x) / d_gauss);
    }

    double af_abs(double a_x)
    {
        return (a_x < 0.0) ? -a_x : a_x;
    }

    double af_sine_signed(double a_x, double freq, double phase)
    {
        return sin(a_x * freq + phase);
    }

    double af_sine_unsigned(double a_x, double freq, double phase)
    {
        double f_output = sin(a_x * freq + phase);
        return (f_output + 1.0) / 2.0;
    }


    double af_linear(double a_x, double cut)
    {
        return (a_x + cut);
    }

    enum NeuronType
    {
        NONE = 0,
        INPUT,
        HIDDEN,
        OUTPUT,
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
    }

    class Neuron
    {
    public:
    	//unsigned int id;
    	vector <double> coordinates;				// 2D defined neuron. Could be defined by a vector of doubles
    	double a_input; 						 	// Sum of all input connection
    	double a_output;							// Sum of all input connection but activated as output of the neuron

    	NeuronType type;							
    	ActivationFunction activation_function;

    	bool outgoing;
    	bool incoming;

    	void Activate()
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
            a_output = y;
    	}

    	bool operator==(Neuron const& other) const
    	{
    		return ((this->c_x == other.c_x) && (this->c_y && other.c_y) && (this->type && other.type));
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
