#ifndef SUBSTRATE_CPP
#define SUBSTRATE_CPP

#include "Substrate.hpp"
using namespace ANN_USM;

Substrate::Substrate(vector < double * > inputs, vector < double * > outputs){
	this->inputs = inputs;
	this->outputs = outputs;	
}
Substrate::Substrate(){

}
Substrate::~Substrate(){
	vector<SpatialNode *>().swap(input_nodes);
	vector<SpatialNode *>().swap(hidden_nodes);
	vector<SpatialNode *>().swap(output_nodes);
}
char * Substrate::SJsonDeserialize(char * substrate_info)
{
	const char delimeters[] = "{\"\t\n:,[ ]}";

	int substrate_type = 0;
	int n_rows = 2;
	int coordenate_type = 0;
	int n_nodes = 0;

	int dataNumber = 0;
		
	if(!strcmp(substrate_info,(char *)"coordenate_type"))
	{
		dataNumber++;

		substrate_info = strtok(NULL, delimeters);
		coordenate_type = atoi(substrate_info);
		substrate_info = strtok(NULL, delimeters);
	}
	if(!strcmp(substrate_info,(char *)"node_function"))
	{
		dataNumber++;

		substrate_info = strtok(NULL, delimeters);

		int size = strlen(substrate_info)+1;
		this->node_function = new char[size];
		strncpy(this->node_function, substrate_info, size);

		substrate_info = strtok(NULL, delimeters);
	}

	if (!strcmp(substrate_info,(char *)"Rows"))
	{
		dataNumber++;

		substrate_info = strtok(NULL, delimeters);
		for(int i = 0; i < 2; i++)
		{
			if(!strcmp(substrate_info,(char *)"n_nodes"))
			{
				substrate_info = strtok(NULL, delimeters);
				n_nodes = atoi(substrate_info);
				substrate_info = strtok(NULL, delimeters);
				if(i == 0 && n_nodes != (int)inputs.size())
				{
					cerr << "HYPERNEAT ERROR:\tThe size of inputs vector mismatch with input nodes number in the substrate configuration. The size of inputs vector is " << (int)inputs.size() << " but should be " << n_nodes << "." << endl;
					exit(0);
				}
				else if(i == n_rows-1 && n_nodes != (int)outputs.size())
				{
					cerr << "HYPERNEAT ERROR:\tThe size of outputs vector mismatch with output nodes number in the substrate configuration. The size of outputs vector is " << (int)outputs.size() << " but should be " << n_nodes << "." << endl;
					exit(0);
				}
			}
			if(!strcmp(substrate_info,(char *)"nodes_info"))
			{
				for(int j = 0; j < n_nodes; j++)
				{
					int node_type;
					int IO_id;
					vector < double > coordenates; 

					substrate_info = strtok(NULL, delimeters);
					node_type = atoi(substrate_info);
					substrate_info = strtok(NULL, delimeters);
					IO_id = atoi(substrate_info);

					for(int k = 0; k < coordenate_type; k++)
					{	
						substrate_info = strtok(NULL, delimeters);
						coordenates.push_back(atof(substrate_info));
					}			

					if(node_type == 0){
						input_nodes.push_back(new SpatialNode(node_type, coordenates, node_function));
						input_nodes.at(j)->SetInputToInputNode(inputs.at(IO_id), IO_id);
					}
					else if(node_type == 2){
						output_nodes.push_back(new SpatialNode(node_type, coordenates, node_function));
						output_nodes.at(j)->SetOutputToOutputNode(outputs.at(IO_id), IO_id);
					}

				}
				substrate_info = strtok(NULL, delimeters);
			}
		}
	}

	if(dataNumber != SUBSTRATE_DATANUMBER)
	{
		cerr << "HYPERNEAT ERROR:\tThe hyperneat config file is not correct" << endl;
		exit(0);
	}
	
	return substrate_info;
}

int Substrate::GetLayerNodesNumber(vector <SpatialNode *> layer)
{
	// Add constraint to verify the existence of the layer. Could be replaceable by one method for each layer in order to avoid the previous.
	return (int)layer.size();
}

SpatialNode * Substrate::GetSpatialNode(vector <SpatialNode *> layer, int node_num)
{
	if(node_num + 1 > (int)layer.size())
	{
		cerr << "ESHYPERNEAT ERROR:\tThe node " << node_num << " in the layer does not exist" << endl;
		return NULL;
	}

	return layer.at(node_num);
}

void Substrate::Activate()
{
	// Iterate over connections in order to obtain its output signal
	for (unsigned int i = 0; i < connections.size(); i++){
		connections[i]->signal = connections[i]->source_node->activation_output * connections[i]->weight;
	}

	// Iterate again over connections passing the output signal to target node sum
	for (unsigned int i = 0; i < connections.size(); i++){
		connections[i]->target_node->activation_sum += connections[i]->signal;
	}

	// Activate hidden nodes with the new input
	for (unsigned int i = 0; i < hidden_nodes.size(); i++){
		hidden_nodes[i]->ActivateNode();
	}

	// Activate output nodes with the new input
	for (unsigned int i = 0; i < output_nodes.size(); i++){
		output_nodes[i]->ActivateNode();
	}
}

void Substrate::EvaluateSpatialNode(vector <SpatialNode *> layer)
{
	for(int i = 0; i < (int)layer.size(); i++){
		layer.at(i)->OutputCalcule();
	}
}

void Substrate::ClearSpatialNodeInputs(vector <SpatialNode *> layer)
{
	for(int i = 0; i < (int)layer.size(); i++){
		layer.at(i)->ClearInputs();
	}
}

void Substrate::ClearSubstrate()
{
	connections.clear();

	ClearSpatialNodeInputs(input_nodes);
	ClearSpatialNodeInputs(hidden_nodes);
	ClearSpatialNodeInputs(output_nodes);

	hidden_nodes.clear();
}

vector <double> Substrate::GetOutputs(){
	vector <double> temp;
	for (unsigned int i = 0; i < output_nodes.size(); i++){
		temp.push_back(output_nodes[i]->GetOutput());
	}

	return temp;
}

vector < string > Substrate::GetSubstrateOutputFunctions()
{
	vector < string > functions;
	
	for(int j = 0; j < (int)output_nodes.size(); j++)
		functions.push_back(output_nodes.at(j)->GetOutputNodeFunction());
	
	return functions;
}
vector < int > Substrate::GetInputOrder()
{
	vector < int > input_order;
	
	for(int j = 0; j < (int)input_nodes.size(); j++)
		input_order.push_back(input_nodes.at(j)->GetInputId());
	
	return input_order;
}

vector < int > Substrate::GetOutputOrder()
{
	vector < int > output_order;
	
	for(int j = 0; j < (int)output_nodes.size(); j++)
		output_order.push_back(output_nodes.at(j)->GetOutputId());
	
	return output_order;
}

string Substrate::getSubstrateConnectionString()
{
	stringstream connections;

	connections << "Layer 0" << endl << "{" << endl;

	for(int j = 0; j < (int)input_nodes.size(); j++)
	{
		connections << "\tNode " << j << endl;
		connections << "\t{" << endl;
		connections << input_nodes.at(j)->getConnectionString();
		connections << "\t}" << endl;
	}

	connections << "}" << endl;

	connections << "Layer 1" << endl << "{" << endl;

	if (((int)hidden_nodes.size()))
	{
		for(int j = 0; j < (int)hidden_nodes.size(); j++)
		{
			connections << "\tNode " << j << endl;
			connections << "\t{" << endl;
			connections << hidden_nodes.at(j)->getConnectionString();
			connections << "\t}" << endl;
		}
	}

	connections << "}" << endl;

	connections << "Layer 2" << endl << "{" << endl;

	for(int j = 0; j < (int)output_nodes.size(); j++)
	{
		connections << "\tNode " << j << endl;
		connections << "\t{" << endl;
		connections << output_nodes.at(j)->getConnectionString();
		connections << "\t}" << endl;
	}

	connections << "}" << endl;


	return connections.str();
}

#endif