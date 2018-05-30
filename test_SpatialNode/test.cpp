#include <vector>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <ESHYPERNEAT>

#define INPUTS 2
#define OUTPUTS 1

using namespace std;
using namespace ANN_USM;

vector <double> inputVector;
vector <double> outputVector;

int main(int argc, char *argv[])
{
	srand((unsigned) time(NULL));
	for (unsigned int i = 0; i < INPUTS; i++){
		inputVector.push_back((double)rand() / RAND_MAX);
	}

	for (unsigned int i = 0; i < OUTPUTS; i++){
		outputVector.push_back(0.0);
	}

	Substrate *t_substrate = new Substrate(inputVector, outputVector);

	vector <double> coordenates1;
	coordenates1.reserve(2);

	coordenates1.push_back(-0.5);
	coordenates1.push_back(0.5);

	vector <double> coordenates2;
	coordenates2.reserve(2);

	coordenates2.push_back(0.5);
	coordenates2.push_back(0.5);

	vector <double> coordenates_i1;
	coordenates_i1.reserve(2);

	coordenates_i1.push_back(1.0);
	coordenates_i1.push_back(-1.0);

	vector <double> coordenates_i2;
	coordenates_i2.reserve(2);

	coordenates_i2.push_back(1.0);
	coordenates_i2.push_back(1.0);

	vector <double> coordenates_o;
	coordenates_o.reserve(2);

	coordenates_o.push_back(0.0);
	coordenates_o.push_back(-1.0);

	/*
	SpatialNode *hidden_node1 = new SpatialNode(1, coordenates1, (char *)"SIGMOID");
	SpatialNode *hidden_node2 = new SpatialNode(1, coordenates2, (char *)"SIGMOID");
	*/
	SpatialNode hidden_node1(1, coordenates1, (char *)"SIGMOID");
	SpatialNode hidden_node2(1, coordenates2, (char *)"SIGMOID");

	SpatialNode input_node1(0, coordenates_i1, (char *)"SIGMOID");
	SpatialNode input_node2(0, coordenates_i2, (char *)"SIGMOID");

	SpatialNode output_node(2, coordenates_o, (char *)"SIGMOID");

	t_substrate->hidden_nodes.push_back(hidden_node1);
	t_substrate->hidden_nodes.push_back(hidden_node2);

	t_substrate->input_nodes.push_back(input_node1);
	t_substrate->input_nodes.push_back(input_node2);
	t_substrate->input_nodes[0].SetInputToInputNode(inputVector[0], 0);
	t_substrate->input_nodes[1].SetInputToInputNode(inputVector[1], 1);

	t_substrate->output_nodes.push_back(output_node);
	t_substrate->output_nodes[0].SetOutputToOutputNode(outputVector[0], 0);

	for (int i = 0; i < t_substrate->input_nodes.size(); i++)
	{
		t_substrate->input_nodes[i].ActivateNode();
		for (int j = 0; j < t_substrate->hidden_nodes.size(); j++)
		{
			Connection t_connection;
			t_connection.source_node = &t_substrate->input_nodes[i];
			t_connection.target_node = &t_substrate->hidden_nodes[j];
			t_connection.weight = (double)rand() / RAND_MAX;

			t_substrate->connections.push_back(t_connection);
		}
	}

	for (int i = 0; i < t_substrate->output_nodes.size(); i++)
	{
		for (int j = 0; j < t_substrate->hidden_nodes.size(); j++)
		{
			Connection t_connection;
			t_connection.source_node = &t_substrate->hidden_nodes[j];
			t_connection.target_node = &t_substrate->output_nodes[i];
			t_connection.weight = (double)rand() / RAND_MAX;	

			t_substrate->connections.push_back(t_connection);
		}
	}

	for (int i = 0; i < t_substrate->connections.size(); i++)
	{
		clog << "Connection_" << i << ": (" << t_substrate->connections[i].source_node->GetCoordenates()[0] << ", " << t_substrate->connections[i].source_node->GetCoordenates()[1] << ") ";
		clog << "to (" << t_substrate->connections[i].target_node->GetCoordenates()[0] << ", " << t_substrate->connections[i].target_node->GetCoordenates()[1] << ") ";
		clog << "weight: " << t_substrate->connections[i].weight << endl;
	}

	for (unsigned int i = 0; i < 10; i++)
		t_substrate->Activate();

	clog << "Net output: " << t_substrate->GetOutputs()[0] << endl;

	return 0;
}