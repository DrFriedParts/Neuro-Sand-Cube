#ifndef __NSC_COMMAND_H__
#define __NSC_COMMAND_H__

#include "Connection.h"

#include <string>
#include <vector>
#include <map>
#include <functional>

#include <boost/shared_ptr.hpp>

typedef std::function<void ()> CommandFunctor;

struct CommandAttributes
{
	std::string id;
	std::vector<std::string> from;  // from which terminals is this allowed
	std::vector<std::string> route; // route through which terminals
};

class CommandController // that sounds cool
{
public:
	CommandController(void);
	~CommandController(void);

	void AddConfiguration(CommandAttributes& attr);
	void AddCommand(std::string id, CommandFunctor);
	void AddCommander(boost::shared_ptr<ConnectionInterface> connection); // because it sounds cool

	void Process(std::string source, std::string message);
	
	// polls commanders for commands & executes them
	void PollCommands();

private:
	std::map<std::string, CommandFunctor > commands;
	std::map<std::string, /*boost::shared_ptr<*/CommandAttributes/*>*/ > commandConfigurations;
	std::map<std::string, std::vector<std::string> > clientCommands; // listen of commands each client is able to send. if a client isnt listed, he can send all
	std::vector<boost::shared_ptr<ConnectionInterface> > commanders;
};

#endif

