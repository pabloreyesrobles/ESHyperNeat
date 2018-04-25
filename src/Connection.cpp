#ifndef CONNECTION_CPP
#define CONNECTION_CPP

#include "Conection.hpp"
using namespace ANN_USM;

Connection::Connection(){

}

Connection::Connection(unsigned int t_id, SpatialNode *t_source, SpatialNode *t_target, double t_weight, bool t_recurrent){
	this->id_connection = t_id;
	this->source_node = t_source;
	this->target_node = t_target;
	this->weight = t_weight;
	this->recurrent_flag = t_recurrent;
}

Connection::~Connection(){
	// Must complete
	free(source_node);
	free(target_node);
}

#endif