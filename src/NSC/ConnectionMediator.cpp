#include "ConnectionMediator.h"
#include "CommandController.h"
#include "StateDistributor.h"


void NetworkConnectionInterface::Send(std::string msg)
{
	connection->Send(msg);
}

std::string NetworkConnectionInterface::Receive()
{
	return connection->Receive();
}

bool NetworkConnectionInterface::CanReceive()
{

	return connection->HasReceived();
}

ConnectionMediator::ConnectionMediator(boost::shared_ptr<CommandController> commandController) 
{
	m_spCommandController = commandController;
}


ConnectionMediator::~ConnectionMediator(void)
{
}


void ConnectionMediator::AddConnection(std::string id, boost::shared_ptr<NetworkConnection> connection)
{
	//connections[id] = connection;
	auto newConnection = boost::shared_ptr<NetworkConnectionInterface>(new NetworkConnectionInterface());
	newConnection->connection = connection;
	newConnection->id = id;

	// add to distributor & command parser

	StateDistributor::GetInstance().AddSubscriber(newConnection);
	m_spCommandController->AddCommander(newConnection);



}