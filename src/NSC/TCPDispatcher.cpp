#include "TCPDispatcher.h"


TCPDispatcher::TCPDispatcher(std::string ip, std::string port) :
	m_TCPClient(ip, port)
{
	//m_TCPClient.Connect();
}


TCPDispatcher::~TCPDispatcher(void)
{
	m_TCPClient.Close();
}


void TCPDispatcher::Send(std::string message)
{
	m_TCPClient.Send(message);
}