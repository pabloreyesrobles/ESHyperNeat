#ifndef ESHYPERNEAT_H
#define ESHYPERNEAT_H

#include <NEAT>
#include "Substrate.hpp"
#include "CPPNInputs.hpp"
#include "NeuralNetwork.hpp"
#include <vector>
#include <queue>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <memory>

#define ESHYPERNEAT_DATANUMBER 5

//ESHyperNeat parameters. Add to configuration file

#define DivisionThreshold	0.5
#define VarianceThreshold	0.03
#define BandThreshold		0.1
#define InitialDepth		2
#define MaxDepth			3
#define IterationLevel		1

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
		ESHyperNeat(vector <double> inputs, vector <double> outputs, char *config_file);

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
		bool createSubstrateConnections(Genetic_Encoding *organism, NeuralNetwork &net);

		/**
		 * \brief Create all substrate connections according to CPPN-NEAT result.
		 * \param Path to the organism of CPPN-NEAT that will create connections in the substrate.
		 * \return The return value is true if the creation of connections was successful and false if it was not.
		 */
		bool createSubstrateConnections(char *path);

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
        class TempConnection
        {
        public:
           	vector <double> source;
            vector <double> target;
            double weight;
            
            TempConnection()
            { 
            	weight = 0;
            }
            
            TempConnection(vector <double> t_source, vector <double> t_target, double t_weight)
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
            
        };
        
        // A quadpoint in the HyperCube.
        class QuadPoint
        {
        public:
            double x;
            double y;
            double width;
            double weight;
            double variance;
            int level;
            
            vector <shared_ptr<QuadPoint>> children;
            
            QuadPoint()
            {
                x = y = width = weight = variance = 0;
                level = 0;
                //children.reserve(4);
            }
            
            QuadPoint(double t_x, double t_y, double t_width, int t_level)
            {
                x = t_x;
                y = t_y;
                width = t_width;
                level = t_level;
                weight = 0.0;
                variance = 0.0;
                //children.reserve(4);
                children.clear();
            }
            
            ~QuadPoint()
            {

            };
        };
        
        void DivideInitialize(vector <double> node, shared_ptr<QuadPoint> root, Genetic_Encoding *organism, bool outgoing);
        
        void PruneAndExtraction(vector <double> node, shared_ptr<QuadPoint> root, Genetic_Encoding *organism, vector <TempConnection> &temp_connections, bool outgoing);

        double QuadPointMean(shared_ptr <QuadPoint> point);
        
        double QuadPointVariance(shared_ptr <QuadPoint> point);
        
        void Clean_Net(vector <Connection> &t_connections, unsigned int node_size, unsigned int io_count);

        //void Clean_QuadPoint(shared_ptr<QuadPoint> t_quad);
	};
}
#endif