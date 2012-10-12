#ifndef __SHARED_STATE_CONFIGREADER_H__
#define __SHARED_STATE_CONFIGREADER_H__


#include "../simplejson/JSON.h"

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <time.h>
#include <vector>

#include <boost/shared_ptr.hpp>

struct StateAttributes;

// Reads the config file
// reads consumer aliases
// reads list of states to send to specific consumers
// reads list of input commands executable by consumer

class StateConfigReader
{
public:
	StateConfigReader() { }
	~StateConfigReader() { }

	void ReadConfig(std::string);

	boost::shared_ptr<StateAttributes> Get(int i);
	


private:

	std::vector<boost::shared_ptr<StateAttributes> > config;

};


#endif