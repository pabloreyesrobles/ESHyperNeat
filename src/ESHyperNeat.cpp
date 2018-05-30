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

ESHyperNeat::ESHyperNeat(vector <double> inputs, vector <double> outputs, char *config_file)
{
	// ============================= READING JSON FILE ============================= //
	ifstream file;
	file.open(config_file);
	string hyperneat_info((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
	// ============================================================================= //	

	substrate = new Substrate(inputs, outputs);
	
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
		cerr << "HYPERNEAT ERROR - ESHyperNeat:\tThe hyperneat config file is not correct" << endl;
		exit(0);
	}

	substrate->connections.reserve(pow(4, MaxDepth) + 2);

	//clog << "HYPERNEAT:\tSuccessful serialization" << endl;
}

bool ESHyperNeat::createSubstrateConnections(Genetic_Encoding *organism)
{
	//////////////////////// ESHyperNeat ///////////////////////////////
	// Review the parameters:
	// initialDepth, maxDepth, varianceThreshold, bandThreshold,
	// iterationLevel, divisionThreshold
	
	substrate->ClearSubstrate();

	okConnections = false;

	unsigned int input_count = substrate->input_nodes.size();
	unsigned int output_count = substrate->output_nodes.size();
	unsigned int c_count = 0;
    unsigned int maxNodes = pow(4, MaxDepth); // ADD PARAMETERS

    shared_ptr<QuadPoint> root;

    vector <TempConnection> temp_connections;
    vector <SpatialNode> temp;
    vector <SpatialNode> unexplored_nodes;
    vector <SpatialNode> hidden_nodes;
    vector <Connection> connections;

    hidden_nodes.reserve(maxNodes);
    temp_connections.reserve(maxNodes + 2);
    connections.reserve(maxNodes + 2);
	
    // Input to hidden node connections
    for (unsigned int i = 0; i < input_count; i++)
    {
    	root = shared_ptr<QuadPoint>(new QuadPoint(0, 0, 1, 1));
    	
    	DivideInitialize(substrate->input_nodes[i], root, organism, true);
    	//clog << "---------------------------------------------" << endl;
		//clog << "Division input " << i <<" completed" << endl;
		temp_connections.clear();
    	PruneAndExtraction(substrate->input_nodes[i], root, organism, temp_connections, true);
    	//clog << "Prune input " << i <<" completed" << endl;
    	//clog << "Temp connections size: " << temp_connections.size() << endl;
    	
    	for (unsigned int j = 0; j < temp_connections.size(); j++)
    	{
    		if (abs(temp_connections[j].weight) > connection_threshold)
    		{ // REVIEW THIS THRESHOLD
    			if (!(temp_connections[j].target).CheckInputNode(temp_connections[j].source))
    			{
    				SpatialNode *temp_node;
    				bool h_exists = false;
    				for (unsigned int h = 0; h < substrate->hidden_nodes.size(); h++)
    				{
    					if (temp_connections[j].target == hidden_nodes[h])
    					{
    						h_exists = true;
    						temp_node = &hidden_nodes[h];
    						break;
    					}
    				}

		    		if (!h_exists) {
		    			SpatialNode new_node = temp_connections[j].target;
    				 	hidden_nodes.push_back(new_node);
    				 	temp_node = &hidden_nodes.back();
    				}
    				//temp_connections[j].target->AddInputToNode(temp_connections[j].source, temp_connections[j].weight);
    				
	    			//clog << "ADDING CONNECTION... ";
	    			//Connection *n_connection = new Connection(c_count++, temp_connections[j].source, temp_connections[j].target, temp_connections[j].weight);
	    			Connection n_connection;
	    			n_connection.id_connection = c_count++;
	    			n_connection.source_node = &substrate->input_nodes[i];
	    			n_connection.target_node = temp_node;
	    			n_connection.weight = temp_connections[j].weight;
	    			n_connection.signal = 0.0;
	    			n_connection.recurrent_flag = false;
	    			connections.push_back(n_connection);
	    			//clog << "CONNECTION ADDED" << endl;
    			}
    		}			
    	}
    	//Clean_Quadpoint(root);
    }

    // Hidden to hidden node connections
    unexplored_nodes = hidden_nodes;
    for (unsigned int i = 0; i < IterationLevel; i++)
    {
    	for (unsigned int j = 0; j < unexplored_nodes.size(); j++)
    	{
    		root = shared_ptr<QuadPoint>(new QuadPoint(0, 0, 1, 1));
    		temp_connections.clear();
    		DivideInitialize(unexplored_nodes[j], root, organism, true);
    		//clog << "Division 2 completed" << endl;
    		PruneAndExtraction(unexplored_nodes[j], root, organism, temp_connections, true);
    		//clog << "Prune 2 completed" << endl;
    		for (unsigned int k = 0; k < temp_connections.size(); k++)
    		{
    			if (temp_connections[k].weight > connection_threshold)
    			{
    				if (!(temp_connections[k].target).CheckInputNode(temp_connections[k].source))
    				{
	    				bool h_exists = false;
	    				SpatialNode *temp_node_source;
	    				SpatialNode *temp_node_target;
	    				/*
	    				for (unsigned int h = 0; h < hidden_nodes.size(); h++)
	    				{
	    					if (temp_connections[k].source == hidden_nodes[h])
	    					{
	    						temp_node_source = &hidden_nodes[h]; // REVIEW THIS
	    					}

	    					if (temp_connections[k].target == hidden_nodes[h])
	    					{
	    						h_exists = true;
	    						temp_node_target = &hidden_nodes[h];
	    					}
	    				}
						*/
						vector <SpatialNode>::iterator it;
			    		it = find(hidden_nodes.begin(), hidden_nodes.end(), temp_connections[k].source);
			    		if (it != hidden_nodes.end())
			    		{
			    			temp_node_source = &(*it);
			    		}

			    		it = find(hidden_nodes.begin(), hidden_nodes.end(), temp_connections[k].target);
			    		if (it != hidden_nodes.end())
			    		{
			    			h_exists = true;
			    			temp_node_target = &(*it);
			    		}
			    		else 
			    			temp.push_back(hidden_nodes[j]);

	    				if (!h_exists){
	    					SpatialNode new_node(1, temp_connections[k].target.GetCoordenates(), (char *)"SIGMOID");
	    					hidden_nodes.push_back(new_node);
	    					temp_node_target = &hidden_nodes.back();
	    				}

	    				//temp_connections[k].target->AddInputToNode(temp_connections[k].source, temp_connections[j].weight);

	    				//clog << "ADDING CONNECTION... ";
		    			//Connection *n_connection = new Connection(c_count++, temp_connections[k].source, temp_connections[k].target, temp_connections[k].weight);
		    			Connection n_connection;
		    			n_connection.id_connection = c_count++;
		    			n_connection.source_node = temp_node_source;
		    			n_connection.target_node = temp_node_target;
		    			n_connection.weight = temp_connections[k].weight;
		    			n_connection.signal = 0.0;
		    			n_connection.recurrent_flag = false;
		    			connections.push_back(n_connection);
		    			//clog << "CONNECTION ADDED" << endl;
	    			}
    			}
    		}
    		//Clean_Quadpoint(root);
    	}
    	temp.clear();
    	unexplored_nodes = temp;
    }

    //clog << "hidden_nodes size: " << hidden_nodes.size() << endl; 
    
    // Hidden to output connections
    for (unsigned int i = 0; i < output_count; i++)
    {
    	root = shared_ptr<QuadPoint>(new QuadPoint(0, 0, 1, 1));
    	temp_connections.clear();
    	DivideInitialize(substrate->output_nodes[i], root, organism, false);
    	PruneAndExtraction(substrate->output_nodes[i], root, organism, temp_connections, false);

    	for (unsigned int j = 0; j < temp_connections.size(); j++)
    	{
    		{ // REVIEW THIS THRESHOLD
    			if (!(temp_connections[j].target).CheckInputNode(temp_connections[j].source))
    			{
    				bool h_exists = false;
    				SpatialNode *temp_node;
    				for (unsigned int h = 0; h < hidden_nodes.size(); h++)
    				{
    					if (temp_connections[j].source == hidden_nodes[h])
    					{
    						h_exists = true;
    						temp_node = &hidden_nodes[h];
    					}
    				}
    				if (h_exists)
    				{
    					//(temp_connections[j].target)->AddInputToNode(temp_connections[j].source, temp_connections[j].weight);
		    			//Connection *n_connection = new Connection(c_count++, temp_connections[j].source, temp_connections[j].target, temp_connections[j].weight);
		    			Connection n_connection;
		    			n_connection.id_connection = c_count++;
		    			n_connection.source_node = temp_node;
		    			n_connection.target_node = &substrate->output_nodes[i];
		    			n_connection.weight = temp_connections[j].weight;
		    			n_connection.signal = 0.0;
		    			n_connection.recurrent_flag = false;
		    			connections.push_back(n_connection);
    				}
    			}
    		}    			
    	}
    	//Clean_Quadpoint(root);
    }
    /*
    clog << "hidden_nodes size: " << hidden_nodes.size() << endl; 
    clog << "connections last size: " << connections.size() << endl;
    
    
    
    for(unsigned int i = 0; i < hidden_nodes.size(); i++){
    	clog << "Listing hidden_" << i << ": (" << hidden_nodes[i].GetCoordenates()[0] << ", " << hidden_nodes[i].GetCoordenates()[1] << ")" << endl;
    }
    
    for(unsigned int i = 0; i < substrate->input_nodes.size(); i++){
    	clog << "Listing input_" << i << ": " << &substrate->input_nodes[i] << endl;
    }
    for(unsigned int i = 0; i < substrate->output_nodes.size(); i++){
    	clog << "Listing output_" << i << ": " << &substrate->output_nodes[i] << endl;
    }
	
	for(unsigned int i = 0; i < connections.size(); i++){
    	clog << "Listing x1: " << connections[i].source_node->GetCoordenates()[0] << " y1: " << connections[i].source_node->GetCoordenates()[1];
		clog << " x2: " << connections[i].target_node->GetCoordenates()[0] << " y2: " << connections[i].target_node->GetCoordenates()[1];
		clog << " weight: " << connections[i].weight << endl;
    }
	*/
    Clean_Net(connections, hidden_nodes);
    //clog << "connections new size: " << connections.size() << endl;
	
    substrate->hidden_nodes = hidden_nodes;
    substrate->connections = connections;
    substrate->ConnectionReferenceUpdate();
    //substrate->printNodes();
    okConnections = true;

    temp_connections.clear();
    unexplored_nodes.clear();
    hidden_nodes.clear();
    temp.clear();
    connections.clear();

    vector<TempConnection>().swap(temp_connections);
    vector<SpatialNode>().swap(unexplored_nodes);
    vector<SpatialNode>().swap(hidden_nodes);
    vector<SpatialNode>().swap(temp);
    vector<Connection>().swap(connections);
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

	// This should be as long as many layers the substrate have. Needs improvement
	for (unsigned int i = 0; i < 10; i++){
		substrate->Activate();
	}

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

	if (myfile.is_open())
	{

		myfile << "function [ ";
		for(int i = 0; i < (int)substrate->outputs.size(); i++)
		{
			myfile << "OUTPUT_" << i ;
			if(i < (int)substrate->outputs.size()-1) myfile << ", ";
		}
		myfile << " ] = HYPERNEAT_OUTPUTS( ";
		for(int i = 0; i < (int)substrate->inputs.size(); i++)
		{
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

void ESHyperNeat::DivideInitialize(SpatialNode node, shared_ptr<QuadPoint> root, Genetic_Encoding *organism, bool outgoing)
{
	vector <double> t_input;
	vector <double> t_output;
	shared_ptr<QuadPoint> p;
	queue <shared_ptr<QuadPoint>> q;

	q.push(root);
	while (!q.empty())
	{
		p = q.front();

		p->children.push_back(shared_ptr<QuadPoint>(new QuadPoint(p->x - p->width/2, p->y - p->width/2, p->width/2, p->level + 1)));
		p->children.push_back(shared_ptr<QuadPoint>(new QuadPoint(p->x - p->width/2, p->y + p->width/2, p->width/2, p->level + 1)));
		p->children.push_back(shared_ptr<QuadPoint>(new QuadPoint(p->x + p->width/2, p->y - p->width/2, p->width/2, p->level + 1)));
		p->children.push_back(shared_ptr<QuadPoint>(new QuadPoint(p->x + p->width/2, p->y + p->width/2, p->width/2, p->level + 1)));

		for (unsigned int i = 0; i < p->children.size(); i++)
		{
			vector <double> t_coordenates = node.GetCoordenates();
			t_input.clear();

			if (outgoing) {
				t_input.push_back(t_coordenates[0]);
				t_input.push_back(t_coordenates[1]);
				t_input.push_back(p->children[i]->x);
				t_input.push_back(p->children[i]->y);
			}

			else {
				t_input.push_back(p->children[i]->x);
				t_input.push_back(p->children[i]->y);
				t_input.push_back(t_coordenates[0]);
				t_input.push_back(t_coordenates[1]);
			}

			for(int c = 0; c < (int)AditionalCPPNInputs.size(); c++)
				t_input.push_back(AditionalCPPNInputs.at(c).Eval(t_input));

			t_output = organism->eval(t_input);
			//clog << "x1: " << t_input[0] << " y1: " << t_input[1] << " x2: " << t_input[2] << " y2: " << t_input[3] << " output: " << scaleWeight(t_output[0]) << endl;
			p->children[i]->weight = scaleWeight(t_output[0]);
		}

		if ((p->level < InitialDepth) || ((p->level < MaxDepth) && QuadPointVariance(p) > DivisionThreshold)) // ADD PARAMETERS
        {
            for (int i = 0; i < 4; i++)
            {
                q.push(p->children[i]);
            }
        }

        q.pop();
	}
}

void ESHyperNeat::PruneAndExtraction(SpatialNode node, shared_ptr<QuadPoint> root, Genetic_Encoding *organism, vector <TempConnection> &temporal_connections, bool outgoing)
{
	if (root->children.empty())
    {
        return;
    }
    else
    {
    	//clog << "Root not empty" << endl;
        for (unsigned int i = 0; i < root->children.size(); i++)
        {        	
        	//clog << "Checking root children variance at level " << root->children[i]->level << ": " << QuadPointVariance(root->children[i]) << endl;
            if (QuadPointVariance(root->children[i]) > VarianceThreshold) // ADD PARAMETERS
            {
                PruneAndExtraction(node, root->children[i], organism, temporal_connections, outgoing);
            }
            // Band Pruning phase.
            else
            {
                double d_left, d_right, d_top, d_bottom;
                vector <double> t_input;
                vector <double> t_output;

                unsigned int root_index = 0;

                vector <double> t_coordenates = node.GetCoordenates();

                if (outgoing)
                {
                	t_input.push_back(t_coordenates[0]);
                	t_input.push_back(t_coordenates[1]);
                	t_input.push_back(root->children[i]->x);
                	t_input.push_back(root->children[i]->y);

                    root_index = t_coordenates.size();
                }

                else
                {
                    t_input.push_back(root->children[i]->x);
                	t_input.push_back(root->children[i]->y);
                	t_input.push_back(t_coordenates[0]);
                	t_input.push_back(t_coordenates[1]);
                }

                for(int c = 0; c < (int)AditionalCPPNInputs.size(); c++)
					t_input.push_back(AditionalCPPNInputs.at(c).Eval(t_input));

                // Left
                t_input[root_index] -= root->width;
                t_output = organism->eval(t_input);
                d_left = abs(root->children[i]->weight - scaleWeight(t_output[0]));
                
                // Right
                t_input[root_index] += 2 * (root->width);
                t_output = organism->eval(t_input);
                d_right = abs(root->children[i]->weight - scaleWeight(t_output[0]));

                // Top
                t_input[root_index] -= root->width;
                t_input[root_index + 1] -= root->width;
                t_output = organism->eval(t_input);
                d_top = abs(root->children[i]->weight - scaleWeight(t_output[0]));

                // Bottom
                t_input[root_index + 1] += 2 * root->width;
                t_output = organism->eval(t_input);
                d_bottom = abs(root->children[i]->weight - scaleWeight(t_output[0]));


                //clog << "Checking BandThreshold in level: " << root->children[i]->level << " Band: " << max(min(d_top, d_bottom), min(d_left, d_right)) << endl;
                
                if (max(min(d_top, d_bottom), min(d_left, d_right)) > BandThreshold) // ADD PARAMETERS
                {
                	//clog << "Adding connection at level: " << root->children[i]->level << "... ";
                	vector <double> n_coordinates;
                	n_coordinates.push_back(root->children[i]->x);
                	n_coordinates.push_back(root->children[i]->y);

                	//SpatialNode n_node = shared_ptr<SpatialNode>(new SpatialNode(1, n_coordinates, (char *)"SIGMOID")); // Must be variable
					SpatialNode n_node(1, n_coordinates, (char *)"SIGMOID");

					TempConnection n_connection;
                    if (outgoing)
                    {
                    	//n_connection = new TempConnection(node, node, scaleWeight(root->children[i]->weight));
                    	
                    	n_connection.source = node;
                    	n_connection.target = n_node;
                    	n_connection.weight = root->children[i]->weight;
                    	//clog << " x1: " << node->GetCoordenates()[0] << " y1: " << node->GetCoordenates()[1] << " x2:" << n_node->GetCoordenates()[0] << " y2: " << n_node->GetCoordenates()[1];
                    	//clog << "weight: " << n_connection.weight << endl;	
                    }
                    else
                    {
                    	//n_connection = new TempConnection(node, node, scaleWeight(root->children[i]->weight));
                    	
  						n_connection.source = n_node;
                    	n_connection.target = node;
                    	n_connection.weight = root->children[i]->weight;
                    	//clog << " x1: " << n_node->GetCoordenates()[0] << " y1: " << n_node->GetCoordenates()[1] << " x2:" << node->GetCoordenates()[0] << " y2: " << node->GetCoordenates()[1];
                    	//clog << " weight: " << n_connection.weight << endl;
                    }
                    
                    temporal_connections.push_back(n_connection);
                    //clog << "Added connection" << endl;
                }
            }
        }
    }
}

double ESHyperNeat::QuadPointMean(shared_ptr<QuadPoint> point)
{
	double sum = 0;

	if (point->children.empty()) 
		return 0.0;

	else 
	{
		for (unsigned int i = 0; i < point->children.size(); i++)
			sum += point->children[i]->weight;
	}

	return sum/(double)point->children.size();
}

double ESHyperNeat::QuadPointVariance(shared_ptr<QuadPoint> point)
{
	double sum = 0;
	double mean = QuadPointMean(point);
	double variance;
	//clog << "children1: " << point->children[0]->weight << " children2: " << point->children[1]->weight << " children3: " << point->children[2]->weight << " children4: " << point->children[3]->weight << endl;

	if (point->children.empty()) 
		return 0.0;

	else 
	{
		for (unsigned int i = 0; i < point->children.size(); i++)
		{
			sum += pow(point->children[i]->weight - mean, 2);
		}
	}
	variance = sum / point->children.size();

	return variance;
}

// Removes all the dangling connections. This still leaves the nodes though,
void ESHyperNeat::Clean_Net(vector <Connection> &t_connections, vector <SpatialNode> &t_nodes)
{
	unsigned int iteration = 1;

    bool loose_connections = true;
    while (loose_connections)
    {
    	for (unsigned int i = 0; i < t_nodes.size(); i++)
	    {
			t_nodes[i].SetIncoming(false);
			t_nodes[i].SetOutgoing(false);
	    }

        // Make sure inputs and outputs are covered.
        for (unsigned int i = 0; i < t_connections.size(); i++)
        {
        	if(t_connections[i].source_node->GetNodeType() == 0)
        	{
        		t_connections[i].source_node->SetOutgoing(true);
        		t_connections[i].source_node->SetIncoming(true);
        	}

        	if(t_connections[i].target_node->GetNodeType() == 2)
        	{
        		t_connections[i].target_node->SetOutgoing(true);
        		t_connections[i].target_node->SetIncoming(true);
        	}
        }

        // Move on to the nodes.
        for (unsigned int i = 0; i < t_connections.size(); i++)
        {
            if (t_connections[i].source_node != t_connections[i].target_node)
            {
            	t_connections[i].source_node->SetOutgoing(true);
            	t_connections[i].target_node->SetIncoming(true);
            }

        }

        loose_connections = false;

        //clog << "t_connections b_" << iteration << ": " << t_connections.size() << endl;
        /*
        for (itr = t_connections.begin(); itr != t_connections.end(); )
        {
        	if (!(itr->target_node->CheckOutgoing() || itr->source_node->CheckIncoming()))
        	{
        		clog << "ERASING x1: " << itr->source_node->GetCoordenates()[0] << " y1: " << itr->source_node->GetCoordenates()[1];
				clog << " x2: " << itr->target_node->GetCoordenates()[0] << " y2: " << itr->target_node->GetCoordenates()[1];
				clog << " weight: " << itr->weight << endl;	

        		itr = t_connections.erase(itr);

        		if (!loose_connections)
                {
                    loose_connections = true;
                }
        	}
        	else
            {
                itr++;
                clog << "NON ERASING x1: " << itr->source_node->GetCoordenates()[0] << " y1: " << itr->source_node->GetCoordenates()[1];
				clog << " x2: " << itr->target_node->GetCoordenates()[0] << " y2: " << itr->target_node->GetCoordenates()[1];
				clog << " weight: " << itr->weight << endl;	
            }
        }
        */
	    for (unsigned int i = 0; i < t_connections.size();)
	    {
	    	if (!t_connections[i].target_node->CheckOutgoing() || !t_connections[i].source_node->CheckIncoming())
        	{
        		//clog << "ERASING x1: " << t_connections[i].source_node->GetCoordenates()[0] << " y1: " << t_connections[i].source_node->GetCoordenates()[1];
				//clog << " x2: " << t_connections[i].target_node->GetCoordenates()[0] << " y2: " << t_connections[i].target_node->GetCoordenates()[1];
				//clog << " weight: " << t_connections[i].weight << endl;	

	    		t_connections.erase(t_connections.begin()+i);

        		if (!loose_connections)
                {
                    loose_connections = true;
                }
        	}
        	else
            {
                
                //clog << "NON ERASING x1: " << t_connections[i].source_node->GetCoordenates()[0] << " y1: " << t_connections[i].source_node->GetCoordenates()[1];
				//clog << " x2: " << t_connections[i].target_node->GetCoordenates()[0] << " y2: " << t_connections[i].target_node->GetCoordenates()[1];
				//clog << " weight: " << t_connections[i].weight;
				//clog << " Incoming: " << t_connections[i].target_node->CheckIncoming() << " Outgoing: " << t_connections[i].target_node->CheckOutgoing() << endl;	
				i++;
            }
	    }
	    
        //clog << "t_connections a_" << iteration++ << ": " << t_connections.size() << endl;
    }
    //clog << "hidden_nodes size: " << t_nodes.size() << endl;
    // Finally remove the nodes not connected
    //vector <SpatialNode>::iterator itr;
    /*
    for (unsigned int i = 0; i < t_nodes.size(); )
    {
    	if (!t_nodes[i].CheckOutgoing() || !t_nodes[i].CheckIncoming()){
    		clog << "Erasing x: (" << t_nodes[i].GetCoordenates()[0] << ", " << t_nodes[i].GetCoordenates()[1] << ")" << endl;
    		t_nodes.erase(t_nodes.begin()+i);
    	}
    	else i++;
    }
    */
    //clog << "hidden_nodes new size: " << t_nodes.size() << endl;
}

/*
void ESHyperNeat::Clean_Quadpoint(shared_ptr<QuadPoint> t_quad){
	if (t_quad->children.size() != 0)
		for (unsigned int i = 0; i < t_quad->children.size(); i++)
			Clean_Quadpoint(t_quad->children[i]);
	free(t_quad);
}	
*/
#endif