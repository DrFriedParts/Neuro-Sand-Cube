#ifndef __TCP_DISPATCHER_H__
#define __TCP_DISPATCHER_H__

#include "messagedispatcher.h"
#include "Networking.h"

class TCPDispatcher :
	public MessageDispatcher
{
public:
	TCPDispatcher(std::string ip, std::string port);
	virtual ~TCPDispatcher(void);

	virtual void Send(std::string msg);

private:
	TCPClient m_TCPClient;  // too specific?

};

#endif
