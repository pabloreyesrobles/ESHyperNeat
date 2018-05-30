#ifndef SPATIALNODE_CPP
#define SPATIALNODE_CPP

#include "SpatialNode.hpp"

using namespace ANN_USM;

SpatialNode::SpatialNode(int node_type, vector <double> coordenates, char *node_function)
{
	this->node_type = node_type;
	this->coordenates = coordenates;

	//int size = strlen(node_function)+1;
	this->node_function = node_function;
	//strncpy(this->node_function, node_function, size);

	this->outgoing = false;
	this->incoming = false;

	this->activation_output = 0;
	this->activation_sum = 0;

	if(!strcmp(node_function,(char *)"SIGMOID"))
	{
		NodeFunction = &SpatialNode::Sigmoid;		
	}
	/*
	else if(!strcmp(node_function,(char *)"USER_CUSTOM"))
	{
		NodeFunction = &SpatialNode::User_custom;
	}
	*/
	else
	{
		cerr << "ERROR: Does not exist any function called " << node_function << endl;
	}

	output = 0.0;

	if(node_type == 0) active = true;
	else active = false;
}

SpatialNode::SpatialNode()
{
	
}

SpatialNode::~SpatialNode()
{
	//delete node_function;
	//free(node_function);
	vector <double>().swap(coordenates);
	vector <double>().swap(inputs_weight);
	vector <SpatialNode *>().swap(inputs_nodes);
}

void SpatialNode::SetInputToInputNode(double input, int input_id)
{
	if (node_type != 0)
	{
		cerr << "ERROR: This node is not of type input" << endl;
		return;
	}

	this->input = input;
	this->input_id = input_id;

	activation_sum = input;
}

void SpatialNode::SetOutputToOutputNode(double output, int output_id)
{
	if (node_type != 2)
	{
		cerr << "ERROR: This node is not of type output" << endl;
		return;
	}

	this->output = output;
	this->output_id = output_id;
}

bool SpatialNode::CheckInputNode(SpatialNode input_node){
	if(!inputs_nodes.empty()){
		for (unsigned int i = 0; i < inputs_nodes.size(); i++){
			if (*inputs_nodes.at(i) == input_node)
				return true;
		}
	}
	
	return false;
}

void SpatialNode::AddInputToNode(SpatialNode input_node, double input_weight)
{
	if(node_type == 0)
	{
		cerr << "ERROR: Can not connect to a node of type input" << endl;
		return;
	}
	
	if(!active) active = input_node.ActiveNode();

	inputs_nodes.push_back(&input_node);
	inputs_weight.push_back(input_weight);
}

void SpatialNode::ClearActivation()
{
	output = 0; // Looking to remove this
	activation_output = 0;
	activation_sum = 0;
}

void SpatialNode::ActivateNode()
{	
	activation_output = (this->*NodeFunction)(activation_sum);
	//clog << "SpatialNode at : (" << GetCoordenates()[0] << ", " << GetCoordenates()[1] << ") activation_sum: " << activation_sum << endl;
	//clog << "SpatialNode at : (" << GetCoordenates()[0] << ", " << GetCoordenates()[1] << ") activation_output: " << activation_output << endl;
}

void SpatialNode::OutputCalcule()
{
	output = (this->*NodeFunction)(activation_sum);
}

vector < double > SpatialNode::GetCoordenates()
{
	return coordenates;
}

int SpatialNode::GetNodeType()
{
	return node_type;
}

double SpatialNode::GetOutput()
{
	//return *output;
	return activation_output;
}

char *SpatialNode::GetNodeFunction()
{
	return node_function;
}

void SpatialNode::ClearInputs()
{	
	inputs_nodes.clear();
	inputs_weight.clear();
	if(node_type != 0) active = false;
}

string SpatialNode::GetOutputNodeFunction()
{
	stringstream function;

	if(node_type == 2) function << "OUTPUT_" << output_id << " = ";

	function << node_function << "( ";

	if(node_type == 0) function << "INPUT_" << input_id;		
	else
	{
		if((int)inputs_nodes.size() > 0)
			for(int i = 0; i < (int)inputs_nodes.size(); i++)
			{
				function << inputs_nodes.at(i)->GetOutputNodeFunction() << "* " << inputs_weight.at(i);
				if( i + 1 < (int)inputs_nodes.size() ) function << " + ";
			}
		else
			function << "0";
	}
		
	function << " ) ";

	return function.str();
}

int SpatialNode::GetInputId()
{
	return input_id;
}

int SpatialNode::GetOutputId()
{
	return output_id;
}

bool SpatialNode::ActiveNode()
{
	return active;
}

bool SpatialNode::CheckOutgoing()
{
	return outgoing;
}

bool SpatialNode::CheckIncoming()
{
	return incoming;
}

void SpatialNode::SetOutgoing(bool t_outgoing)
{
	this->outgoing = t_outgoing;
}

void SpatialNode::SetIncoming(bool t_incoming)
{
	this->incoming = t_incoming;
}

double SpatialNode::Sigmoid(double input)
{
	return SIGMOID(input);
}

string SpatialNode::getConnectionString()
{
	stringstream connections;

	if(!inputs_nodes.empty())
	{
		for(int i = 0; i < (int)inputs_nodes.size(); i++)
			connections << "\t\tConnection " << i << ":\tWeight\t" << inputs_weight.at(i) << endl;
	}
	else if (node_type == 0)
		connections << "\t\tThis is an input node" << endl;
	else
		connections << "\t\tThis node has no connections" << endl;

	return connections.str();
}

bool SpatialNode::operator == (const SpatialNode &node) const
{
    return this->node_type == node.node_type && this->coordenates[0] == node.coordenates[0] && this->coordenates[1] == node.coordenates[1];
}

bool SpatialNode::operator != (const SpatialNode &node) const
{
    return !(this->node_type == node.node_type && this->coordenates == node.coordenates);
}

void SpatialNode::operator = (const SpatialNode &node)
{
    this->node_type = node.node_type;
	this->coordenates = node.coordenates;

	//int size = strlen(node.node_function)+1;
	this->node_function = node.node_function;
	//strncpy(this->node_function, node.node_function, size);

	this->outgoing = node.outgoing;
	this->incoming = node.incoming;

	this->activation_output = node.activation_output;
	this->activation_sum = node.activation_sum;

	if(!strcmp(node_function,(char *)"SIGMOID"))
	{
		NodeFunction = &SpatialNode::Sigmoid;		
	}
	else
	{
		cerr << "ERROR: Does not exist any function called " << node_function << endl;
	}

	output = 0.0;

	if(node_type == 0) active = true;
	else active = false;
}

#endif