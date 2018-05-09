#ifndef SUBSTRATE_H
#define SUBSTRATE_H

#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>

#include <iostream>
#include <sstream>
#include <fstream>

#include "SpatialNode.hpp"
#include "Connection.hpp"

#define SUBSTRATE_DATANUMBER 3

using namespace std;
/**
 * \namespace ANN_USM
 * \brief Dedicated to artificial intelligence development in Santa María University.
 */
namespace ANN_USM
{
	/**
	 * \class Substrate
	 * \brief The Substrate class is used to implement the net of nodes for an neural network
	 */
	class Substrate
	{			
		char * node_function;
		
	public:
		// Vector of nodes in each layer of each layout in the substrate. There is one layer for each node type.
		vector <SpatialNode *> input_nodes; 
		vector <SpatialNode *> hidden_nodes;
		vector <SpatialNode *> output_nodes;

		vector <Connection *> connections;

		vector< double * > inputs;/**< Vector of inputs pointer of Substrate */
		vector< double * > outputs;/**< Vector of outputs pointer of Substrate */

		/**
		 * \brief Constructor with parameters
		 * \param inputs Vector of all substrate inputs
		 * \param outputs Vector of all substrate outputs
		 */
		Substrate(vector < double * > inputs, vector < double * > outputs);

		/**
		 * \brief Void constructor
		 */
		Substrate();

		/**
		 * \brief Destructor
		 */
		~Substrate();

		/**
		 * \brief Extract all Substrate information of char pointer
		 * \param substrate_info char pointer of json string
		 */
		char * SJsonDeserialize(char * substrate_info);

		/**
		 * \brief Get the node number of a specific layer
		 * \param layer_num Layer number
		 * \return Layer node number
		 */
		int GetLayerNodesNumber(vector <SpatialNode *> layer);

		/**
		 * \brief Get a spatial node of a specific layer and of a specific position in this layer
		 * \param layer_num Layer number
		 * \param layer_node_num Layer node number
		 * \return Indicated spatial node pointer
		 */
		SpatialNode * GetSpatialNode(vector <SpatialNode *> layer, int layer_node_num);

		/**
		 * \brief Perform an activation of the entire substrate. Should be done many times as layers in the substrate.
		*/
		void Activate();

		/**
		 * \brief Evaluate spatial node outputs of a specific layer
		 * \param layer_num Layer number
		 */
		void EvaluateSpatialNode(vector <SpatialNode *> layer);

		/**
		 * \brief Clear of input vector of all spatial nodes in a specific layer
		 * \param layer_num Layer number
		 */
		void ClearSpatialNodeInputs(vector <SpatialNode *> layer);

		void ClearSubstrate();

		void UpdateInputs();

		void Flush();

		vector <double> GetOutputs();

		/**
		 * \brief Allows obtain all final functions of every output node
		 * \return Vector with all final output functions in string format
		 */
		vector < string > GetSubstrateOutputFunctions();

		/**
		 * \brief Allows obtain the input order of all input nodes in the substrate
		 * \return Vector with the input order of all input nodes
		 */
		vector < int > GetInputOrder();

		/**
		 * \brief Allows obtain the output order of all output nodes in the substrate
		 * \return Vector with the output order of all output nodes
		 */
		vector < int > GetOutputOrder();

		/**
		 * \brief This function return a string with information of all input conections of all nodes of each layer.
		 * \return String with information of input conections of the substrate.
		 */
		string getSubstrateConnectionString();
	};	
}
#endif
