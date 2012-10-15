#include "MessageDispatchController.h"

#include "MessageDispatcher.h"


MessageDispatchController::MessageDispatchController(void) 
{
	
}

MessageDispatchController::~MessageDispatchController(void)
{
}

MessageDispatchController& MessageDispatchController::GetInstance()
{
	static MessageDispatchController instance;
	return instance;
}

void MessageDispatchController::AddDispatcher(std::string description, boost::shared_ptr<MessageDispatcher> dispatcher)
{
	m_DispatchPool[description] = dispatcher;
}

boost::shared_ptr<MessageDispatcher> MessageDispatchController::FindDispatcher(std::string description)
{
	auto iterator = m_DispatchPool.find(description);

	if (iterator != m_DispatchPool.end())
		return iterator->second;
	else
		return boost::shared_ptr<MessageDispatcher>();

}

bool MessageDispatchController::HasDispatcher(std::string description)
{
	if (FindDispatcher(description).get() != NULL)
		return true;
	return false;
}


void MessageDispatchController::Send(std::string message, std::string destination)
{

	// okay hacking this in here for now, to get it running with server
	// network connection.  Will change soon, simply have different connection
	// managers, tcp, serial (etc)
	// they will manage sending to their specific ports

	static int first = 0;
	if (first == 0)
	{
		
		++first;
	}
	
	boost::shared_ptr<MessageDispatcher> dispatcher = FindDispatcher(destination);

	if (dispatcher.get() != NULL)
	{
		dispatcher->Send( message);
	}
	else
	{
		// log could not find service
		
		//TEMP!
		//TCPServer::GetInstance().Send(destination, message);
		//m_spServer->Send(destination, message);
	}
}