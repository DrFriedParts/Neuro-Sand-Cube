#include "MessageDispatchController.h"

#include "TCPDispatcher.h"


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
	boost::shared_ptr<MessageDispatcher> dispatcher = FindDispatcher(destination);

	if (dispatcher.get() != NULL)
	{
		dispatcher->Send( message);
	}
	else
	{
		// log could not find service
	}
}