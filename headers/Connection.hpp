#ifndef CONNECTION_H
#define CONNECTION_H

#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>

#include <iostream>
#include <sstream>
#include <fstream>

#include "SpatialNode.hpp"

using namespace std;

namespace ANN_USM
{
	/**
	 * \class Connection
	 * \brief The connection class to identify two Spatial Nodes 
	 */
	class Connection
	{
	public:
		unsigned int id_connection; // Maybe not neccesary
		SpatialNode *source_node;
		SpatialNode *target_node;
		double weight;
		bool recurrent_flag;

		/**
		 * \brief Void constructor
		 */
		Connection();

		/**
		 * \brief Constructor with parameters
		 * \param t_id Identifier of connection
		 * \param t_source Source spatial node
		 * \param t_target Target spatial node
		 * \param t_weight Weight of connection
		 * \param t_recurrent True if connection is recurrent
		 */
		Connection(unsigned int t_id, SpatialNode *t_source, SpatialNode *t_target, double t_weight, bool t_recurrent = false);

		/**
		 * \brief Destructor
		 */
		~Connection();

		bool operator==(Connection const& other) const
	    {
	        if ((this->source_node == other.source_node) && (this->target_node == other.target_node))
	            return true;
	        else
	            return false;
	    }
	};
}
#endif

	