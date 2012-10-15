#pragma once

#include "Networking.h"
#include <string>
#include <boost/shared_ptr.hpp>

class MessageDispatcher
{
public:
	MessageDispatcher(std::string description);
	virtual ~MessageDispatcher(void);

	virtual void Send(std::string msg)  ;

private:

	boost::shared_ptr<NetworkConnection> m_spPort;

};

