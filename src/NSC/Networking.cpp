#include "Networking.h"
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>

boost::asio::io_service TCPClient::m_IOService;

TCPClient::TCPClient(std::string address, std::string port) :
	m_Socket(m_IOService), 
	m_bConnected(false),
	m_bConnecting(false),
	m_bAddressResolved(false),
	m_sAddress(address),
	m_sPort(port)
{
	Init();
}

TCPClient::~TCPClient()
{

}

void TCPClient::Connect()
{
	if (!m_bConnected && !m_bConnecting)
	{	
		m_bConnecting = true;
		m_Socket.async_connect(*m_EndPointIterator, boost::bind(&TCPClient::_ConnectHandler, this, boost::asio::placeholders::error));
		++m_EndPointIterator;
	}
	
}

void TCPClient::_ResolveAddress()
{
	m_bAddressResolved = false;
	tcp::resolver resolver(m_IOService);
	tcp::resolver::query query(m_sAddress,m_sPort);
	try
	{
		m_EndPointIterator = resolver.resolve(query);
		m_EndPointIteratorBegin = m_EndPointIterator;
		m_bAddressResolved = true;
	}
	catch (...)
	{
		m_bAddressResolved = false;
	}
	
	
}

void TCPClient::_ResolveAddressAsync()
{
	m_bAddressResolved = false;
	tcp::resolver resolver(m_IOService);
	tcp::resolver::query query(m_sAddress,m_sPort);
	resolver.async_resolve(query, boost::bind(&TCPClient::_ResolveAddressHandler, this, boost::asio::placeholders::error,  boost::asio::placeholders::iterator));
}

void TCPClient::_ResolveAddressHandler(const boost::system::error_code& errorCode, tcp::resolver::iterator endpointIterator)
{
	if (errorCode == 0)
	{
		m_bAddressResolved = true;
		m_EndPointIterator = endpointIterator;
		m_EndPointIteratorBegin = m_EndPointIterator;
	}
	else
	{
		m_bAddressResolved = false;	
	}
}

void TCPClient::Init()
{
	try 
	{
		_ResolveAddress();
		if (m_bAddressResolved)
			Connect();	
	}
	catch (...)
	{
		int b=0;
	}

}

void TCPClient::_ConnectHandler(const boost::system::error_code& errorCode)
{
	m_bConnecting = false;
	if (errorCode == 0)
	{
		// connection successful
		m_bConnected = true;

		// log connection

	}
	else if (m_EndPointIterator != tcp::resolver::iterator())
	{
		// couldnt connect - try next ip resolution
		m_Socket.close();
		Connect();
	}
	else
	{
		m_Socket.close();
		std::cout << errorCode.value() << std::endl; 
		std::cout << errorCode.category().name() << std::endl; 
		// failed to connect

	}
		
}


void TCPClient::Close()
{
	m_IOService.post(boost::bind(&TCPClient::_CloseConnection, this));
}

void TCPClient::_CloseConnection()
{
	m_Socket.close();
	m_bConnected = false;
	m_bAddressResolved = false;	
	m_bConnecting = false;
}

	
void TCPClient::Send(std::string message)
{
	if (m_bConnected)
	{
		m_sMessage = message;
		_SendHeader();
	}
	else
	{
		if (!m_bAddressResolved)
			_ResolveAddressAsync();
		else if (!m_bConnecting)
		{
			m_EndPointIterator = m_EndPointIteratorBegin;
			Connect();
		}
	}
	
}

void TCPClient::_SendHeader()
{
	std::ostringstream ss;
	ss.width(4);
	ss.fill('0');
	ss <<  m_sMessage.length();
	std::string s(ss.str());
	const char*  c = s.c_str();
	boost::asio::async_write(m_Socket, boost::asio::buffer(c,4), boost::bind(&TCPClient::_SendHeaderHandler,this, boost::asio::placeholders::error));
}

void TCPClient::_SendHeaderHandler(const boost::system::error_code& errorCode)
{
	if (errorCode == 0)
	{
		_Send();
	}
	else
	{
		// log error
		// check type and maybe try again? for now, just close
		_CloseConnection();
	}
}

void TCPClient::_Send()
{
	boost::asio::async_write(m_Socket, boost::asio::buffer(m_sMessage), boost::bind(&TCPClient::_SendHandler,this, boost::asio::placeholders::error));
}



void TCPClient::_SendHandler(const boost::system::error_code& errorCode)
{
	if (errorCode == 0)
	{

	}
	else
	{
		// log error
		// check type and maybe try again? for now, just close
		_CloseConnection();
	}
}

