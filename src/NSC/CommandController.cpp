#include "CommandController.h"

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

#include "../simplejson/JSON.h"
#include "EZLogger.h"



CommandController::CommandController(void)
{

}


CommandController::~CommandController(void)
{

}

void CommandController::AddCommander(boost::shared_ptr<ConnectionInterface> connection)
{
	commanders.push_back(connection);
	connection->Receive();
}


void CommandController::AddConfiguration(CommandAttributes& attr)
{
	//auto a = boost::shared_ptr<CommandAttributes>(new CommandAttributes(attr));
	commandConfigurations[attr.id] = attr;
	for (auto i = attr.from.begin(); i != attr.from.end(); i++)
	{
		clientCommands[*i].push_back(attr.id);
	}
}

void CommandController::AddCommand(std::string id, CommandFunctor f)
{
	commands[id] = f;
}

void CommandController::Process(std::string source, std::string message)
{
	// commands can perform an effect on the game via the command functors ...

	JSONValue *value = JSON::Parse(message.c_str());

	std::string id,target;
	if (value)
	{
		if (!value->IsObject())
		{
			EZLOGGERVLSTREAM(axter::log_always) << "Input from " << source << ": Object expected." <<  std::endl;
		}
		else
		{
			JSONObject object = value->AsObject();

			JSONValue* jsonID = (object.find(L"id") != object.end())?  object[L"id"] : NULL;
			JSONValue* jsonTarget = (object.find(L"target") != object.end())?  object[L"target"] : NULL;

			if (jsonID != NULL && jsonID->IsString())
			{
				std::wstring ws = jsonID->AsString();
				id  = std::string( ws.begin(), ws.end() );
			}
			else
			{
				EZLOGGERVLSTREAM(axter::log_always) << "Input from " << source << ": string id expected." <<  std::endl;
			}

			if (jsonTarget != NULL && jsonTarget->IsString())
			{
				std::wstring ws = jsonTarget->AsString();
				target  = std::string( ws.begin(), ws.end() );
			}
		}
	}
	
	delete value;

	bool toSend = true;
	auto clientCommandIter = clientCommands.find(source);

	if (clientCommandIter != clientCommands.end())
	{
		// there is a filter list for this client
		toSend = false;
		for (auto i = clientCommandIter->second.begin(); i != clientCommandIter->second.end(); i++)
		{
			if (id.compare(*i) == 0 )
			{
				toSend = true;
				break;
			}
		}
	}

	if (!toSend)
		return;

	auto commandIterator = commands.find(id);
	if (commandIterator != commands.end())
	{
		// call the behavior
		(commandIterator->second)(target);
	}

	// ... and they can also be routed to other clients
	auto configurationIter = commandConfigurations.find(id);
	if (configurationIter != commandConfigurations.end())
	{
		// a config exists, send to all in 'route'
		auto configuration = configurationIter->second;
		for (auto i = configuration.route.begin(); i != configuration.route.end(); i++)
		{
			auto client = *i;
			for (auto j = commanders.begin(); j != commanders.end(); j++)
			{
				auto clientConnection = *j;
				if (clientConnection->id.compare(client) == 0)
				{
					clientConnection->Send(message);
				}
			}
		}
	}



}

void CommandController::PollCommands()
{
	for (auto iterator = commanders.begin(); iterator != commanders.end(); iterator++)
	{
		
		auto connection = *iterator;

		if (connection->CanReceive())
		{
			std::string cmd = connection->Receive();
			cmd.erase(std::remove_if(cmd.begin(), cmd.end(), std::isspace), cmd.end());
			Process(connection->id, cmd);
		}
	}
}

