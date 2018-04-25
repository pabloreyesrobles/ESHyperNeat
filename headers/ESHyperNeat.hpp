#ifndef ESHYPERNEAT_H
#define ESHYPERNEAT_H

#include <NEAT>
#include "Substrate.hpp"
#include "Connection.hpp"
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
	 * \class ESHyperNeat
	 * \brief The ESHyperNeat class is used to implement a neuroevolution method called ESHyperNeat.
	 */
	class ESHyperNeat
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

		Substrate * substrate;/**< ESHyperNeat substrate */

		/**
		 * \brief Constructor with parameters.
		 * \param inputs Input vector.
		 * \param outputs Output vector.
		 * \param EShyperneat_info_file Json file.
		 */
		ESHyperNeat(vector < double * > inputs, vector < double * > outputs, char * config_file);

		/**
		 * \brief Destructor.
		 */
		~ESHyperNeat();

		/**
		 * \brief Extract all ESHyperNeat information of json string.
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

		/////////////////////////////////////////////
        // Evolvable Substrate HyperNEAT
        
        // Temporal connection between two spatial nodes
        struct TempConnection
        {
           	SpatialNode *source;
            SpatialNode *target;
            double weight;
            
            TempConnection()
            { 
            	weight = 0;
            }
            
            TempConnection(SpatialNode *t_source, SpatialNode *t_target, double t_weight)
            {
                source = t_source;
                target = t_target;
                weight = t_weight;
            }
            
            ~TempConnection()
            {

            };
            
            bool operator==(const TempConnection &rhs) const
            {
                return (source == rhs.source && target == rhs.target);
            }
            
            bool operator!=(const TempConnection &rhs) const
            {
                return (source != rhs.source && target != rhs.target);
            }
        };
        
        // A quadpoint in the HyperCube.
        struct QuadPoint
        {
            double x;
            double y;
            double width;
            double weight;
            double variance;
            int level;
            
            vector < QuadPoint * > children;
            
            QuadPoint()
            {
                x = y = width = weight = variance = 0;
                level = 0;
                children.reserve(4);
            }
            
            QuadPoint(double t_x, double t_y, double t_width, int t_level)
            {
                x = t_x;
                y = t_y;
                width = t_width;
                level = t_level;
                weight = 0.0;
                variance = 0.0;
                children.reserve(4);
                children.clear();
            }
            
            ~QuadPoint()
            {

            };
        };
        
        void DivideInitialize(SpatialNode *node, QuadPoint *root, Genetic_Encoding *organism, const bool &outgoing);
        
        void PruneAndExtraction(SpatialNode *node, QuadPoint *root, Genetic_Encoding *organism, vector <TempConnection *> connections, const bool &outgoing);

        double QuadPointMean(QuadPoint *point);
        
        double QuadPointVariance(QuadPoint *point);
        
        void Clean_Net(vector <Connection> *t_connections, unsigned int input_count, unsigned int output_count, unsigned int hidden_count);
	};
}
#endif