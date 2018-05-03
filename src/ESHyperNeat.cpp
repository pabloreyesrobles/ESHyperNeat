#ifndef ESHYPERNEAT_CPP
#define ESHYPERNEAT_CPP

#include "ESHyperNeat.hpp"
using namespace ANN_USM;

double ESHyperNeat::scaleWeight(double weight)
{
	if (weight > 0.0)
	{
		return (double)(max_connection_weight/(1.0 - connection_threshold))*(weight - connection_threshold);	
	}else
	{
		return (double)((max_connection_weight/(1.0 - connection_threshold))*(weight + 1) - max_connection_weight);
	}
	
}

ESHyperNeat::ESHyperNeat(vector < double * > inputs, vector < double * > outputs, char * config_file)
{
	// ============================= READING JSON FILE ============================= //
	ifstream file;
	file.open(config_file);
	string hyperneat_info((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
	// ============================================================================= //	

	substrate = new Substrate(inputs,outputs);
	
	hyperNeatJsonDeserialize(hyperneat_info);	
}

ESHyperNeat::~ESHyperNeat()
{
	free(substrate);
	vector<CPPNInputs>().swap(AditionalCPPNInputs);
}

void ESHyperNeat::hyperNeatJsonDeserialize(string hyperneat_info)
{
	int dataNumber = 0;

	char str[(int)hyperneat_info.size()];
	strcpy(str, hyperneat_info.c_str());
	const char delimeters[] = "{\"\t\n:,[ ]}";
	char *pch = strtok(str, delimeters);
	
	if(!strcmp(pch,(char *)"n_AditionalCPPNInputs"))
	{
		dataNumber++;

		pch = strtok(NULL, delimeters);
		int n_AditionalCPPNInputs = atoi(pch);
		pch = strtok(NULL, delimeters);

		if(!strcmp(pch,(char *)"AditionalCPPNInputs"))
		{
			dataNumber++;

			for(int i = 0; i < n_AditionalCPPNInputs; i++)
			{
				pch = strtok(NULL, delimeters);		
				if (!strcmp(pch,(char *)"BIAS"))
				{	
					char * aux = pch;
					pch = strtok(NULL, delimeters);						
					AditionalCPPNInputs.push_back(CPPNInputs(aux, atof(pch)));
				}else																
					AditionalCPPNInputs.push_back(CPPNInputs(pch));
			}						
			pch = strtok(NULL, delimeters);					
		}
	}
	
	if(!strcmp(pch,(char *)"connection_threshold"))
	{
		dataNumber++;

		pch = strtok(NULL, delimeters);
		connection_threshold = atof(pch);
		pch = strtok(NULL, delimeters);
	}
	if(!strcmp(pch,(char *)"max_connection_weight"))
	{
		dataNumber++;

		pch = strtok(NULL, delimeters);
		max_connection_weight = atof(pch);
		pch = strtok(NULL, delimeters);
	}
	if (!strcmp(pch,(char *)"Substrate"))
	{	
		dataNumber++;

		pch = strtok(NULL, delimeters);
		pch = substrate->SJsonDeserialize(pch);
	}

	if(dataNumber != ESHYPERNEAT_DATANUMBER)
	{
		cerr << "HYPERNEAT ERROR:\tThe hyperneat config file is not correct" << endl;
		exit(0);
	}

	clog << "HYPERNEAT:\tSuccessful serialization" << endl;
}

bool ESHyperNeat::createSubstrateConnections(Genetic_Encoding * organism)
{
	//////////////////////// ESHyperNeat ///////////////////////////////
	// Considerar y trabajar en los parámetros del algoritmo:
	// initialDepth, maxDepth, varianceThreshold, bandThreshold,
	// iterationLevel, divisionThreshold

	unsigned int input_count = substrate->input_nodes.size();
	unsigned int output_count = substrate->output_nodes.size();
	unsigned int c_count = 0;
    //unsigned int maxNodes = pow(4, MaxDepth); // ADD PARAMETERS

    QuadPoint *root;

    vector <TempConnection *> temp_connections;
    vector <SpatialNode *> temp;
    vector <SpatialNode *> unexplored_nodes;

    //hidden_nodes.reserve(maxNodes);
    //connections.reserve(maxNodes + 2);

    // Input to hidden node connections
    for (unsigned int i = 0; i < input_count; i++){
    	root = new QuadPoint(0, 0, 1, 1);
    	temp_connections.clear();
    	DivideInitialize(substrate->input_nodes.at(i), root, organism, true);
    	PruneAndExtraction(substrate->input_nodes.at(i), root, organism, temp_connections, true);

    	for (unsigned int j = 0; j < temp_connections.size(); j++){
    		if (temp_connections[j]->weight > connection_threshold){ // REVIEW THIS THRESHOLD
    			if (!(temp_connections[j]->target)->CheckInputNode(temp_connections[j]->source)){
    				(temp_connections[j]->target)->AddInputToNode(temp_connections[j]->source, temp_connections[j]->weight);
	    			substrate->hidden_nodes.push_back(temp_connections[j]->target);

	    			Connection *n_connection = new Connection(c_count++, temp_connections[j]->source, temp_connections[j]->target, temp_connections[j]->weight);
	    			substrate->connections.push_back(n_connection);
    			}
    		}    			
    	}
    }

    // Hidden to hidden node connections
    unexplored_nodes = substrate->hidden_nodes;
    for (unsigned int i = 0; i < IterationLevel; i++){
    	for (unsigned int j = 0; j < unexplored_nodes.size(); j++){
    		root = new QuadPoint(0, 0, 1, 1);
    		temp_connections.clear();
    		DivideInitialize(unexplored_nodes.at(j), root, organism, true);
    		PruneAndExtraction(unexplored_nodes.at(j), root, organism, temp_connections, true);
    		for (unsigned int k = 0; k < temp_connections.size(); k++){
    			if (temp_connections[k]->weight > connection_threshold){
    				if (!(temp_connections[k]->target)->CheckInputNode(temp_connections[k]->source)){
	    				(temp_connections[k]->target)->AddInputToNode(temp_connections[k]->source, temp_connections[k]->weight);
		    			substrate->hidden_nodes.push_back(temp_connections[k]->target);

		    			Connection *n_connection = new Connection(c_count++, temp_connections[j]->source, temp_connections[j]->target, temp_connections[j]->weight);
	    				substrate->connections.push_back(n_connection);
	    			}
    			}
    		}
    	}
    	temp.clear();
    	for (unsigned int j = 0; j < substrate->hidden_nodes.size(); j++){
    		vector <SpatialNode *>::iterator it;
    		it = find(unexplored_nodes.begin(), unexplored_nodes.end(), substrate->hidden_nodes.at(j));
    		if (it != unexplored_nodes.end())
    			continue;
    		temp.push_back(substrate->hidden_nodes.at(j));
    	}
    	unexplored_nodes = temp;
    }
    
    // Hidden to output connections
    for (unsigned int i = 0; i < output_count; i++){
    	root = new QuadPoint(0, 0, 1, 1);
    	temp_connections.clear();
    	DivideInitialize(substrate->output_nodes.at(i), root, organism, false);
    	PruneAndExtraction(substrate->output_nodes.at(i), root, organism, temp_connections, false);

    	for (unsigned int j = 0; j < temp_connections.size(); j++){
    		if (temp_connections[j]->weight > connection_threshold){ // REVIEW THIS THRESHOLD
    			if (!(temp_connections[j]->target)->CheckInputNode(temp_connections[j]->source)){
    				vector <SpatialNode *>::iterator it;
    				it = find(substrate->hidden_nodes.begin(), substrate->hidden_nodes.end(), temp_connections[j]->source);
    				if (it != substrate->hidden_nodes.end()){
    					(temp_connections[j]->target)->AddInputToNode(temp_connections[j]->source, temp_connections[j]->weight);

    					Connection *n_connection = new Connection(c_count++, temp_connections[j]->source, temp_connections[j]->target, temp_connections[j]->weight);
	    				substrate->connections.push_back(n_connection);
    				}
    			}
    		}    			
    	}
    }

    Clean_Net(substrate->connections);
    return true; // Add cases which the phenotype construction could fail
}

bool ESHyperNeat::createSubstrateConnections(char * path)
{
	Genetic_Encoding * organism = new Genetic_Encoding();
	organism->load(path);

	return createSubstrateConnections(organism);	
}

bool ESHyperNeat::evaluateSubstrateConnections()
{
	if(substrate->input_nodes.size() == 0 && substrate->output_nodes.size() == 0)
	{
		cerr << "HYPERNEAT ERROR:\tDoes not exist any substrate initialized" << endl;
		return false;
	}

	if(!okConnections) 
	{
		cerr << "HYPERNEAT ERROR:\tDoes not exist any active output node" << endl;
		return false;
	}

	substrate->EvaluateSpatialNode(substrate->input_nodes);
	substrate->EvaluateSpatialNode(substrate->hidden_nodes);
	substrate->EvaluateSpatialNode(substrate->output_nodes);

	return true;
}

void ESHyperNeat::getHyperNeatOutputFunctions(Genetic_Encoding * organism)
{
	if(!createSubstrateConnections(organism))
	{
		cerr << "HYPERNEAT ERROR:\tThe output function can not be returned" << endl;
		return;
	}

	if(system("mkdir -p ./functions_files") == -1)
	{
		cerr << "HYPERNEAT ERROR:\tFailed to create folder 'functions_files'" << endl;
		return;
	}

	if(system("rm -f ./functions_files/*") == -1)
	{
		cerr << "HYPERNEAT ERROR:\tFailed to remove files inside of 'functions_files' folder" << endl;
		return;
	}

	vector < string > OUTPUTS;

	OUTPUTS = substrate->GetSubstrateOutputFunctions();	
		
	stringstream file_name;
	file_name << "functions_files/HYPERNEAT_OUTPUTS.m";
	ofstream myfile (file_name.str().c_str());

	getNodeFunction();

	if (myfile.is_open()){

		myfile << "function [ ";
		for(int i = 0; i < (int)substrate->outputs.size(); i++){
			myfile << "OUTPUT_" << i ;
			if(i < (int)substrate->outputs.size()-1) myfile << ", ";
		}
		myfile << " ] = HYPERNEAT_OUTPUTS( ";
		for(int i = 0; i < (int)substrate->inputs.size(); i++){
			myfile << "INPUT_" << i ;
			if(i < (int)substrate->inputs.size()-1) myfile << ", ";
		}
		myfile << " )" << endl;
		for(int i = 0; i < (int)substrate->outputs.size(); i++)
			myfile << OUTPUTS.at(i) << ";" << endl;
	    myfile.close();
  	}else
  	{
  		cerr << "HYPERNEAT ERROR:\tUnable to open file: " << file_name.str() << endl;	
  		return;
  	}

  	clog << "HYPERNEAT:\tHyperNeat output function was created successfully" << endl;
}

void ESHyperNeat::getHyperNeatOutputFunctions(char * path)
{
	Genetic_Encoding * organism = new Genetic_Encoding();
	organism->load(path);

	getHyperNeatOutputFunctions(organism);
}

void ESHyperNeat::getNodeFunction()
{
	ofstream myfile("functions_files/SIGMOID.m");

	if (myfile.is_open())
	{
    	myfile << OCTAVE_SIGMOID_STATEMENT << endl;
	    myfile << OCTAVE_SIGMOID_CONST_LETTER << " = " << OCTAVE_SIGMOID_CONST << ";" << endl;
	    myfile << OCTAVE_SIGMOID_FUNC << ";" << endl;
	    myfile.close();

  	}else 
  		cerr << "HYPERNEAT ERROR:\tUnable to open file: functions_files/SIGMOID.m" << endl;	
}

void ESHyperNeat::printConnectionFile(Genetic_Encoding * organism, const char fileName[])
{
	if(!createSubstrateConnections(organism))
	{
		cerr << "HYPERNEAT ERROR:\tThe connection file can not be printed" << endl;
		return;
	}

	ofstream myfile (fileName);

	if(myfile.is_open())
	{
		myfile << substrate->getSubstrateConnectionString() << endl;
	}
	else
	{
		cerr << "HYPERNEAT ERROR:\tUnable to create the file: " << fileName << endl;
		return;
	}

	myfile.close();

	clog << "HYPERNEAT:\tHyperNeat connection file was printed successfully" << endl;
}

void ESHyperNeat::printConnectionFile(char * path, const char fileName[])
{
	Genetic_Encoding * organism = new Genetic_Encoding();
	organism->load(path);

	printConnectionFile(organism, fileName);
}

void ESHyperNeat::DivideInitialize(SpatialNode *node, QuadPoint *root, Genetic_Encoding *organism, const bool &outgoing){
	vector <double> t_input;
	vector <double> t_output;
	QuadPoint *p;
	queue <QuadPoint *> q;

	q.push(root);
	while (!q.empty()){
		p = q.front();

		p->children.push_back(new QuadPoint(p->x - p->width/2, p->y - p->width/2, p->width/2, p->level + 1));
		p->children.push_back(new QuadPoint(p->x - p->width/2, p->y + p->width/2, p->width/2, p->level + 1));
		p->children.push_back(new QuadPoint(p->x + p->width/2, p->y - p->width/2, p->width/2, p->level + 1));
		p->children.push_back(new QuadPoint(p->x + p->width/2, p->y + p->width/2, p->width/2, p->level + 1));

		for (int i = 0; p->children.size(); i++){
			vector <double> t_coordenates = node->GetCoordenates();

			if (outgoing) {
				t_input.insert(t_input.end(), t_coordenates.begin(), t_coordenates.end());
				t_input.insert(t_input.end(), p->children.at(i)->x, p->children.at(i)->y);
			}

			else {
				t_input.insert(t_input.end(), p->children.at(i)->x, p->children.at(i)->y);
				t_input.insert(t_input.end(), t_coordenates.begin(), t_coordenates.end());
			}

			for(int c = 0; c < (int)AditionalCPPNInputs.size(); c++)
				t_input.push_back(AditionalCPPNInputs.at(c).Eval(t_input));

			t_output = organism->eval(t_input);
			p->children[i]->weight = t_output[0];
		}

		if ((p->level < InitialDepth) || ((p->level < MaxDepth) && QuadPointVariance(p) > DivisionThreshold)) // ADD PARAMETERS
        {
            for (int i = 0; i < 4; i++)
            {
                q.push(p->children.at(i));
            }
        }
        q.pop();
	}
}

void ESHyperNeat::PruneAndExtraction(SpatialNode *node, QuadPoint *root, Genetic_Encoding *organism, vector <TempConnection *> temp_connections, const bool &outgoing){
	if (root->children[0] == NULL)
    {
        return;
    }

    else
    {
        for (unsigned int i = 0; i < 4; i++)
        {
            if (QuadPointVariance(root->children[i]) > VarianceThreshold) // ADD PARAMETERS
            {
                PruneAndExtraction(node, root->children[i], organism, temp_connections, outgoing);
            }

            // Band Pruning phase.
            else
            {
                double d_left, d_right, d_top, d_bottom;
                vector <double> t_input;
                vector <double> t_output;

                int root_index = 0;

                vector <double> t_coordenates = node->GetCoordenates();

                if (outgoing)
                {
                    t_input.insert(t_input.end(), t_coordenates.begin(), t_coordenates.end());
					t_input.insert(t_input.end(), root->children.at(i)->x, root->children.at(i)->y);

                    root_index = t_coordenates.size();
                }

                else
                {
                    t_input.insert(t_input.end(), root->children.at(i)->x, root->children.at(i)->y);
					t_input.insert(t_input.end(), t_coordenates.begin(), t_coordenates.end());
                }

                for(int c = 0; c < (int)AditionalCPPNInputs.size(); c++)
					t_input.push_back(AditionalCPPNInputs.at(c).Eval(t_input));

                // Left
                t_input[root_index] -= root->width;
                t_output = organism->eval(t_input);
                d_left = abs(root->children[i]->weight - t_output[0]);
                
                // Right
                t_input[root_index] += 2 * (root->width);
                t_output = organism->eval(t_input);
                d_right = abs(root->children[i]->weight - t_output[0]);

                // Top
                t_input[root_index] -= root->width;
                t_input[root_index + 1] -= root->width;
                t_output = organism->eval(t_input);
                d_top = abs(root->children[i]->weight - t_output[0]);

                // Bottom
                t_input[root_index + 1] += 2 * root->width;
                t_output = organism->eval(t_input);
                d_bottom = abs(root->children[i]->weight - t_output[0]);
                
                if (max(min(d_top, d_bottom), min(d_left, d_right)) > BandThreshold) // ADD PARAMETERS
                {
                	vector <double> n_coordinates;
                	n_coordinates.insert(n_coordinates.end(), root->children.at(i)->x, root->children.at(i)->y);

                	SpatialNode *n_node = new SpatialNode(1, n_coordinates, "SIGMOID"); // Must be variable
                	TempConnection *t_connection;

                    if (outgoing)
                    {
                        t_connection = new TempConnection(node, n_node, root->children[i]->weight);
                    }
                    else
                    {
                        t_connection = new TempConnection(n_node, node, root->children[i]->weight);
                    }

                    temp_connections.push_back(t_connection);
                }
            }
        }
    }
}

double ESHyperNeat::QuadPointMean(QuadPoint *point){
	double sum = 0;

	if (point->children[0] == NULL) 
		return 0.0;

	else 
	{
		for (unsigned int i = 0; i < point->children.size(); i++)
			sum += point->children[i]->weight;
	}

	return sum / point->children.size();
}

double ESHyperNeat::QuadPointVariance(QuadPoint *point){
	double sum = 0;
	double mean = QuadPointMean(point);

	if (point->children[0] == NULL) 
		return 0.0;

	else 
	{
		for (unsigned int i = 0; i < point->children.size(); i++)
			sum += pow(point->children[i]->weight - mean, 2);
	}

	return sum / point->children.size();
}

// Removes all the dangling connections. This still leaves the nodes though,
void ESHyperNeat::Clean_Net(vector <Connection *> t_connections)
{
    bool loose_connections = true;
    while (loose_connections)
    {
        // Make sure inputs and outputs are covered.
        for (unsigned int i = 0; i < t_connections.size(); i++)
        {
        	if(t_connections.at(i)->source_node->GetNodeType() == 0){
        		t_connections.at(i)->source_node->SetOutgoing(true);
        		t_connections.at(i)->source_node->SetIncoming(true);
        	}

        	if(t_connections.at(i)->target_node->GetNodeType() == 2){
        		t_connections.at(i)->target_node->SetOutgoing(true);
        		t_connections.at(i)->target_node->SetIncoming(true);
        	}
        }

        // Move on to the nodes.
        for (unsigned int i = 0; i < t_connections.size(); i++)
        {
            if (t_connections.at(i)->source_node != t_connections.at(i)->target_node){
            	t_connections.at(i)->source_node->SetOutgoing(true);
            	t_connections.at(i)->target_node->SetIncoming(true);
            }

        }

        loose_connections = false;

        vector <Connection *>::iterator itr;
        for (itr = t_connections.begin(); itr != t_connections.end();)
        {
        	if (!(*itr)->target_node->CheckOutgoing() || !(*itr)->source_node->CheckIncoming()){
        		itr = t_connections.erase(itr);
        		if (!loose_connections)
                {
                    loose_connections = true;
                }
        	}
        	else
            {
                itr++;
            }
        }
    }
}

#endif