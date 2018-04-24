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
	okConnections = false;

	if(substrate->(int)input_nodes.size() == 0 && substrate->(int)output_nodes.size())
	{
		cerr << "HYPERNEAT ERROR:\tDoes not exist any substrate initialized" << endl;
		return false;
	}

	if(substrate->GetLayersNumber() - 1 != organism->getNEATOutputSize())
	{
		cerr << "HYPERNEAT ERROR:\tThe layout number does not correspond to the cppn output number" << endl;
		return false;
	}

	for(int i = 0; i < substrate->GetLayersNumber()-1; i++)
	{				
		substrate->ClearSpatialNodeInputs(i+1);

		for(int j = 0; j < substrate->GetLayerNodesNumber(i); j++)
			for(int k = 0; k < substrate->GetLayerNodesNumber(i+1); k++)
			{
				vector < double > cppn_inputs;
				vector < double > cppn_output;
				vector < double > c1 = (substrate->GetSpatialNode(i,j))->GetCoordenates();
				vector < double > c2 = (substrate->GetSpatialNode(i+1,k))->GetCoordenates();
				cppn_inputs.insert(cppn_inputs.end(), c1.begin(), c1.end());
				cppn_inputs.insert(cppn_inputs.end(), c2.begin(), c2.end());

				for(int c = 0; c < (int)AditionalCPPNInputs.size(); c++)
					cppn_inputs.push_back(AditionalCPPNInputs.at(c).Eval(cppn_inputs));
				
				cppn_output = organism->eval(cppn_inputs);

				if(abs(cppn_output.at(i)) > connection_threshold)
					(substrate->GetSpatialNode(i+1,k))->AddInputToNode(substrate->GetSpatialNode(i,j), scaleWeight(cppn_output.at(i)));
			}
	}		
	
	for(int i = 0; i < substrate->GetLayersNumber(); i++)
		for(int j = 0; j < substrate->GetLayerNodesNumber(i); j++)
			if((substrate->GetSpatialNode(i,j))->GetNodeType() == 2 && (substrate->GetSpatialNode(i,j))->ActiveNode())
			{
				okConnections = true;
				//clog << "HYPERNEAT:\tConnections successfully created" << endl;
				return true; 
			}

	cerr << "HYPERNEAT ERROR:\tDoes not exist any active output node" << endl;
	return false;

	//////////////////////// ESHyperNeat ///////////////////////////////
	// Considerar y trabajar en los parámetros del algoritmo:
	// initialDepth, maxDepth, varianceThreshold, bandThreshold,
	// iterationLevel, divisionThreshold
	
}

bool ESHyperNeat::createSubstrateConnections(char * path)
{
	Genetic_Encoding * organism = new Genetic_Encoding();
	organism->load(path);

	return createSubstrateConnections(organism);	
}

bool ESHyperNeat::evaluateSubstrateConnections()
{
	if(substrate->(int)input_nodes.size() == 0 && substrate->(int)output_nodes.size() == 0)
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
			p->children->weight = t_output[0];
		}

		if ((p->level < InitialDepth) || ((p->level < MaxDepth) && Variance(p) > DivisionThreshold)) // ADD PARAMETERS
        {
            for (int i = 0; i < 4; i++)
            {
                q.push(p->children.at(i));
            }
        }
        q.pop();
	}
}

void ESHyperNeat::PruneAndExtraction(SpatialNode *node, QuadPoint *root, Genetic_Encoding *organism, const bool &outgoing){
	if (root->children[0] == NULL)
    {
        return;
    }

    else
    {
        for (unsigned int i = 0; i < 4; i++)
        {
            if (Variance(root->children[i]) > VarianceThreshold) // ADD PARAMETERS
            {
                PruneExpress(node, root->children[i], organism, connections, outgoing);
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
                d_left = Abs(root->children[i]->weight - t_output[0]); // DEFINE ABS
                
                // Right
                t_input[root_index] += 2 * (root->width);
                t_output = organism->eval(t_input);
                d_right = Abs(root->children[i]->weight - t_output[0]); // DEFINE ABS

                // Top
                t_input[root_index] -= root->width;
                t_input[root_index + 1] -= root->width;
                t_output = organism->eval(t_input);
                d_top = Abs(root->children[i]->weight - t_output[0]); // DEFINE ABS

                // Bottom
                t_input[root_index + 1] += 2 * root->width;
                t_output = organism->eval(t_input);
                d_bottom = Abs(root->children[i]->weight - t_output[0]); // DEFINE ABS
                
                if (max(min(d_top, d_bottom), (d_left, d_right)) > BandThreshold) // ADD PARAMETERS
                {
                	vector <double> n_coordinates;
                	n_coordinates.insert(n_coordinates.end(), root->children.at(i)->x, root->children.at(i)->y);

                	SpatialNode *n_node = new SpatialNode(1, n_coordinates, substrate->node_function);
                	substrate->hidden_nodes.push_back(n_node);

                    if (outgoing)
                    {
                        n_node->AddInputToNode(node, root->children[i]->weight);
                    }
                    else
                    {
                        node->AddInputToNode(n_node, root->children[i]->weight);
                    }
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
		for (int i = 0; i < point->children.size(); i++)
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
		for (int i = 0; i < point->children.size(); i++)
			sum += pow(point->children[i]->weight - mean, 2);
	}

	return sum / point->children.size();
}

#endif