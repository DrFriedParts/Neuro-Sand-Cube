#pragma once

#include <string>


class MessageDispatcher
{
public:
	MessageDispatcher(void) { }
	virtual ~MessageDispatcher(void) { }

	virtual void Send(std::string msg) = 0 ;
};

