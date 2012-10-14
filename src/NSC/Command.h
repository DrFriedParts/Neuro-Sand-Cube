#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <string>
#include <vector>

struct CommandAttributes
{
	std::string id;
	std::vector<std::string> from;  // from which terminals is this allowed
	std::vector<std::string> route; // route through which terminals
};

class Command
{
public:
	Command(void);
	~Command(void);
};

#endif

