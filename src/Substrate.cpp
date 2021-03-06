#ifndef SUBSTRATE_CPP
#define SUBSTRATE_CPP

#include "Substrate.hpp"
using namespace ANN_USM;

Substrate::Substrate(vector <double> inputs, vector <double> outputs){
	this->inputs = inputs;
	this->outputs = outputs;	
}

Substrate::Substrate(){

}
Substrate::~Substrate(){
	vector<SpatialNode>().swap(input_nodes);
	vector<SpatialNode>().swap(hidden_nodes);
	vector<SpatialNode>().swap(output_nodes);
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

					SpatialNode new_node(node_type, coordenates, node_function);
					/*
					SpatialNode new_node;
					new_node.node_type = node_type;
					new_node.coordenates = coordenates;
					new_node.node_function = node_function;
					*/
					if(node_type == 0){
						input_nodes.push_back(new_node);
						input_nodes[j].SetInputToInputNode(inputs[IO_id], IO_id);
					}
					else if(node_type == 2){
						output_nodes.push_back(new_node);
						output_nodes[j].SetOutputToOutputNode(outputs[IO_id], IO_id);
					}

				}
				substrate_info = strtok(NULL, delimeters);
			}
		}
	}

	if(dataNumber != SUBSTRATE_DATANUMBER)
	{
		cerr << "HYPERNEAT ERROR - Substrate:\tThe hyperneat config file is not correct" << endl;
		exit(0);
	}
	
	return substrate_info;
}

int Substrate::GetLayerNodesNumber(vector <SpatialNode> layer)
{
	// Add constraint to verify the existence of the layer. Could be replaceable by one method for each layer in order to avoid the previous.
	return (int)layer.size();
}
/*
SpatialNode &Substrate::GetSpatialNode(vector <SpatialNode> layer, int node_num)
{
	if(node_num + 1 > (int)layer.size())
	{
		cerr << "ESHYPERNEAT ERROR:\tThe node " << node_num << " in the layer does not exist" << endl;
		return NULL;
	}

	return &layer[node_num];
}
*/

void Substrate::EvaluateSpatialNode(vector <SpatialNode> layer)
{
	for(int i = 0; i < (int)layer.size(); i++){
		layer[i].OutputCalcule();
	}
}

void Substrate::ClearSpatialNodeInputs(vector <SpatialNode> layer)
{
	for(int i = 0; i < (int)layer.size(); i++){
		layer[i].ClearInputs();
	}
}

void Substrate::UpdateInputs(vector <double> t_input){
	for (unsigned int i = 0; i < input_nodes.size(); i++){
		input_nodes[i].activation_sum = t_input[i];
		input_nodes[i].ActivateNode();
	}
}

void Substrate::Flush()
{
	for (unsigned int i = 0; i < input_nodes.size(); i++){
		input_nodes[i].ClearActivation();
	}

	for (unsigned int i = 0; i < hidden_nodes.size(); i++){
		hidden_nodes[i].ClearActivation();
	}

	for (unsigned int i = 0; i < output_nodes.size(); i++){
		output_nodes[i].ClearActivation();
	}
}

bool Substrate::CheckIfHiddenExists(SpatialNode t_node)
{
	for (unsigned int i = 0; i < hidden_nodes.size(); i++){
		if (t_node == hidden_nodes[i])
			return true;			
	}
	return false;
}

vector <double> Substrate::GetOutputs()
{
	vector <double> temp;
	for (unsigned int i = 0; i < output_nodes.size(); i++){
		temp.push_back(output_nodes[i].GetOutput());
	}

	return temp;
}

vector < string > Substrate::GetSubstrateOutputFunctions()
{
	vector < string > functions;
	
	for(int j = 0; j < (int)output_nodes.size(); j++)
		functions.push_back(output_nodes[j].GetOutputNodeFunction());
	
	return functions;
}
vector < int > Substrate::GetInputOrder()
{
	vector < int > input_order;
	
	for(int j = 0; j < (int)input_nodes.size(); j++)
		input_order.push_back(input_nodes[j].GetInputId());
	
	return input_order;
}

vector < int > Substrate::GetOutputOrder()
{
	vector < int > output_order;
	
	for(int j = 0; j < (int)output_nodes.size(); j++)
		output_order.push_back(output_nodes[j].GetOutputId());
	
	return output_order;
}

string Substrate::getSubstrateConnectionString()
{
	stringstream s_connections;

	s_connections << "Layer 0" << endl << "{" << endl;

	for(int j = 0; j < (int)input_nodes.size(); j++)
	{
		s_connections << "\tNode " << j << endl;
		s_connections << "\t{" << endl;
		s_connections << input_nodes[j].getConnectionString();
		s_connections << "\t}" << endl;
	}

	s_connections << "}" << endl;

	s_connections << "Layer 1" << endl << "{" << endl;

	if (((int)hidden_nodes.size()))
	{
		for(int j = 0; j < (int)hidden_nodes.size(); j++)
		{
			s_connections << "\tNode " << j << endl;
			s_connections << "\t{" << endl;
			s_connections << hidden_nodes[j].getConnectionString();
			s_connections << "\t}" << endl;
		}
	}

	s_connections << "}" << endl;

	s_connections << "Layer 2" << endl << "{" << endl;

	for(int j = 0; j < (int)output_nodes.size(); j++)
	{
		s_connections << "\tNode " << j << endl;
		s_connections << "\t{" << endl;
		s_connections << output_nodes[j].getConnectionString();
		s_connections << "\t}" << endl;
	}

	s_connections << "}" << endl;


	return s_connections.str();
}

void Substrate::printNodes(){
	clog << "Input nodes: " << endl;
	for (unsigned int i = 0; i < input_nodes.size(); i++){
		clog << "(" << input_nodes[i].GetCoordenates()[0] << ", ";
		clog << input_nodes[i].GetCoordenates()[1] << ")" << endl;
	}

	clog << "Hidden nodes: " << endl;
	for (unsigned int i = 0; i < hidden_nodes.size(); i++){
		clog << "(" << hidden_nodes[i].GetCoordenates()[0] << ", ";
		clog << hidden_nodes[i].GetCoordenates()[1] << ")" << endl;
	}

	clog << "Output nodes: " << endl;
	for (unsigned int i = 0; i < output_nodes.size(); i++){
		clog << "(" << output_nodes[i].GetCoordenates()[0] << ", ";
		clog << output_nodes[i].GetCoordenates()[1] << ")" << endl;
	}
}

#endif