#ifndef HYPERNEAT_H
#define HYPERNEAT_H

#include <NEAT>
#include "Substrate.hpp"
#include "CPPNInputs.hpp"
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>

#define HYPERNEAT_DATANUMBER 5

using namespace std;
/**
 * \namespace ANN_USM
 * \brief Dedicated to artificial intelligence development in Santa María University.
 */
namespace ANN_USM{
	/**
	 * \class HyperNeat
	 * \brief The HyperNeat class is used to implement a neuroevolution method called HyperNeat.
	 */
	class HyperNeat
	{
		// Vector of aditional cppn inputs
		vector < CPPNInputs > AditionalCPPNInputs;

		//Threshold that determine the creation for an connection
		double connection_threshold;
		double max_connection_weight;

		//Indicated if the connections were created successfully
		bool okConnections;

		double scaleWeight(double weight);

	public:

		Substrate * substrate;/**< hyperNeat substrate */

		/**
		 * \brief Constructor with parameters.
		 * \param inputs Input vector.
		 * \param outputs Output vector.
		 * \param hyperneat_info_file Json file.
		 */
		HyperNeat(vector < double * > inputs, vector < double * > outputs, char * config_file);

		/**
		 * \brief Destructor.
		 */
		~HyperNeat();

		/**
		 * \brief Extract all HyperNeat information of json string.
		 * \param hyperneat_info json string.
		 */
		void hyperNeatJsonDeserialize(string hyperneat_info);

		/**
		 * \brief Create all substrate connections according to CPPN-NEAT result.
		 * \param organism Organism of CPPN-NEAT that will create connections in the substrate.
		 * \return The return value is true if the creation of connections was successful and false if it was not.
		 */
		bool createSubstrateConnections(Genetic_Encoding * organism);

		/**
		 * \brief Create all substrate connections according to CPPN-NEAT result.
		 * \param Path to the organism of CPPN-NEAT that will create connections in the substrate.
		 * \return The return value is true if the creation of connections was successful and false if it was not.
		 */
		bool createSubstrateConnections(char * path);

		/**
		 * \brief Allows to obtain the final HyperNeat outputs.
		 * \return The return value is true if the evaluation of connections was successful and false if it was not.
		 */
		bool evaluateSubstrateConnections();
		
		/**
		 * \brief Allows obtain all final functions of every output node. This functions is formated for its use in Octave or Matlab environment.
		 * \param organism Organism of CPPN-NEAT that will create connections in the substrate.
		 */
		void getHyperNeatOutputFunctions(Genetic_Encoding * organism);

		/**
		 * \brief Allows obtain all final functions of every output node. This functions is formated for its use in Octave or Matlab environment.
		 * \param Path to the organism of CPPN-NEAT that will create connections in the substrate.
		 */
		void getHyperNeatOutputFunctions(char * path);

		/**
		 * \brief Allows obtain the output node function used in all SpatialNodes. This functions is formated for its use in Octave or Matlab environment.
		 */
		void getNodeFunction();

		/**
		 * \brief This functions allows obtain the file with th information of all conections of all substrate nodes.
		 * \param organism Organism of CPPN-NEAT that will create connections in the substrate.
		 */
		void printConnectionFile(Genetic_Encoding * organism, const char fileName[] = "HyperNeat_Connections.txt");

		/**
		 * \brief This functions allows obtain the file with th information of all conections of all substrate nodes.
		 * \param Path to the organism of CPPN-NEAT that will create connections in the substrate.
		 */
		void printConnectionFile(char * path, const char fileName[] = "HyperNeat_Connections.txt");
	};
}
#endif