#ifndef __SHARED_STATE_CONFIGREADER_H__
#define __SHARED_STATE_CONFIGREADER_H__


#include "../simplejson/JSON.h"

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <time.h>
#include <vector>
#include <map>

#include <boost/shared_ptr.hpp>

// forward declares
struct StateAttributes;
struct CommandAttributes;

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
	
	void _ReadAliases(JSONValue* value);
	void _ReadInputs(JSONValue *value);
	void _ReadOutputs(JSONValue *value);

	std::vector<std::string> _ReadSources(JSONValue* value); 

	std::string _WStringToString(std::wstring wstr) { return std::string( wstr.begin(), wstr.end()); };

	std::map<std::string, std::string> m_Aliases;
	std::vector<boost::shared_ptr<StateAttributes> > config;
	std::vector<boost::shared_ptr<CommandAttributes> > m_CommandConfig;

};


#endif