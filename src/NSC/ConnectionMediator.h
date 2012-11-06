#ifndef __CONNECTION_MEDIATOR_H__
#define __CONNECTION_MEDIATOR_H__

#include <map>

#include <boost/shared_ptr.hpp>

#include "Networking.h"
#include "Connection.h"

// fwd decl
class CommandController;

struct NetworkConnectionInterface : public ConnectionInterface
{
	virtual void Send(std::string);
	virtual std::string Receive();
	virtual bool CanReceive();

	boost::shared_ptr<NetworkConnection> connection;
};


class ConnectionMediator 
{
public:
	ConnectionMediator(boost::shared_ptr<CommandController> commandController);
	~ConnectionMediator(void);

	void AddConnection(std::string id, boost::shared_ptr<NetworkConnection> connection);

private:

	std::map<std::string, boost::shared_ptr<ConnectionInterface> > connections;

	boost::shared_ptr<CommandController> m_spCommandController;
	
};

#endif


