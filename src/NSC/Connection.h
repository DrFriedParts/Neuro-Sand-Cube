#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <string>

struct ConnectionInterface
{
	virtual void Send(std::string) =0;
	virtual bool CanReceive() = 0;
	virtual std::string Receive() = 0;
	std::string id;
};


#endif

