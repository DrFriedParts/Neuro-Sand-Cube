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

struct SharedStateAttributes;

// Reads the config file
// extracts all necessary shared state to be streamed somewhere
// and adds them to SharedStateController 
class SharedStateConfigReader
{
public:
	SharedStateConfigReader() { }
	~SharedStateConfigReader() { }

	void ReadConfig(std::string);

	boost::shared_ptr<SharedStateAttributes> Get(int i);


private:

	std::vector<boost::shared_ptr<SharedStateAttributes> > config;

};


#endif