#include <vector>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <ESHYPERNEAT>

#define INPUTS 3
#define OUTPUTS 1

using namespace std;
using namespace ANN_USM;

vector <double *> inputVector;
vector <double *> outputVector;

int main(int argc, char *argv[])
{
	for (unsigned int i = 0; i < INPUTS; i++){
		inputVector.push_back(new double);
	}

	for (unsigned int i = 0; i < OUTPUTS; i++){
		outputVector.push_back(new double);
	}

	Substrate *t_substrate = new Substrate(inputVector, outputVector);

	vector <double> coordenates1;
	coordenates1.reserve(2);

	coordenates1.push_back(1.0);
	coordenates1.push_back(2.5);

	vector <double> coordenates2;
	coordenates2.reserve(2);

	coordenates2.push_back(1.0);
	coordenates2.push_back(2.5);

	clog << "coordenates1: (" << coordenates1[0] << ", " << coordenates1[1] << ")" << endl;
	clog << "coordenates2: (" << coordenates2[0] << ", " << coordenates2[1] << ")" << endl;

	SpatialNode *node1 = new SpatialNode(1, coordenates1, (char *)"SIGMOID");
	SpatialNode *node2 = new SpatialNode(1, coordenates2, (char *)"SIGMOID");

	node1->activation_sum = 30;

	t_substrate->hidden_nodes.push_back(node1);

	clog << "SpatialNode1 coordenates: (" << node1->GetCoordenates()[0] << ", " << node1->GetCoordenates()[1] << ") activation_sum: " << node1->activation_sum << endl;
	clog << "SpatialNode1 coordenates: (" << node2->GetCoordenates()[0] << ", " << node2->GetCoordenates()[1] << ") activation_sum: " << node2->activation_sum << endl;

	if (t_substrate->CheckIfHiddenExists(node2)){
		clog << "EQUAL" << endl;
		//node2 = node1;
	}
	else clog << "NOT EQUAL" << endl;

	clog << "SpatialNode1 coordenates: (" << node1->GetCoordenates()[0] << ", " << node1->GetCoordenates()[1] << ") activation_sum: " << node1->activation_sum << endl;
	clog << "SpatialNode1 coordenates: (" << node2->GetCoordenates()[0] << ", " << node2->GetCoordenates()[1] << ") activation_sum: " << node2->activation_sum << endl;
	
	if (node1 == node2){
		clog << "EQUAL" << endl;
	}
	else clog << "NOT EQUAL" << endl;

	return 0;
}