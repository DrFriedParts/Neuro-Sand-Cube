#pragma once

#include "MessageDispatcher.h"

#include <string>
#include <map>

#include <boost/shared_ptr.hpp>


// works with pool of messengers
// provides public interface for sending messages

// currently this class uses a map with strings as keys to find the correct dispatchers
// this may be a bit intense, if so find another way by using int ids

class MessageDispatchController
{
public:
	
	~MessageDispatchController(void);

	static MessageDispatchController& GetInstance();

	void AddDispatcher(std::string description, boost::shared_ptr<MessageDispatcher> dispatcher);

	boost::shared_ptr<MessageDispatcher> FindDispatcher(std::string description);
	bool HasDispatcher(std::string description);

	void Send(std::string message, std::string destination);

private:
	
	MessageDispatchController();
	std::map<std::string, boost::shared_ptr<MessageDispatcher> > m_DispatchPool;
};



