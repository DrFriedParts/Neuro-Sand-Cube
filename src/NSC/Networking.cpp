#include "Networking.h"
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../ezlogger/ezlogger_headers.hpp"



boost::asio::io_service NetworkConnection::m_IOService;

//boost::asio::io_service TCPServer::m_IOService;

/*TCPClient::TCPClient(std::string address, std::string port) :
	m_Socket(m_IOService), 
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
		EZLOGGERVLSTREAM(axter::log_rarely) << "Transmission of message header failed! " << m_sAddress << ":" <<  m_sPort <<  std::endl;
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

		EZLOGGERVLSTREAM(axter::log_always) << "Connected successfully " << m_sAddress << ":" <<  m_sPort <<  std::endl;
	
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

		EZLOGGERVLSTREAM(axter::log_always) << "Failed to open network socket " << m_sAddress << ":" <<  m_sPort <<  "(" ;//<< errorCode.value() << ", " << errorCode.category().name() << ") "<< std::endl;
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
		EZLOGGERVLSTREAM(axter::log_rarely) << "Transmission of message header failed! " << m_sAddress << ":" <<  m_sPort <<  std::endl;
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
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
// Serial port
////


SerialPort::SerialPort(std::string port, int iBaudRate, SerialPort_Parity eParity, int iDataBits, float fStopBits ) :
	m_Port(m_IOService), 
	m_sPort(port),
	m_iBaudRate(iBaudRate),
	m_eParity(eParity),
	m_iDataBits(iDataBits),
	m_fStopBits(fStopBits)
{
	Init();
}

SerialPort::~SerialPort()
{
	_CloseConnection();
}


void SerialPort::Connect()
{
	try
	{
		m_Port.open(m_sPort);
	
		if (!m_Port.is_open())
		{
			EZLOGGERVLSTREAM(axter::log_rarely) << "Failed to open serial port - " << m_sPort <<"!"<< std::endl;
		}

		m_bConnected = true;

		m_Port.set_option(boost::asio::serial_port::baud_rate(m_iBaudRate));
		if (m_eParity == SP_PARITY_NONE)
			m_Port.set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none));
		else if (m_eParity == SP_PARITY_ODD)
			m_Port.set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::odd));
		else
			m_Port.set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::even));
	
		if (m_fStopBits == 1.0f)
			m_Port.set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));
		else if (m_fStopBits < 2.0f)
			m_Port.set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::onepointfive));
		else
			m_Port.set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::two));

		m_Port.set_option(boost::asio::serial_port::character_size(m_iDataBits));
	}
	catch (...)
	{
		m_bConnected = false;

		EZLOGGERVLSTREAM(axter::log_rarely) << "Failed to open serial port " << m_sPort <<  std::endl;
	}
}

void SerialPort::Init()
{
	Connect();

}


void SerialPort::Close()
{
	m_IOService.post(boost::bind(&SerialPort::_CloseConnection, this));
}

void SerialPort::_CloseConnection()
{
	//m_Port.shutdown();
	m_Port.close();
	m_bConnected = false;
	m_bConnecting = false;
}

	
void SerialPort::Send(std::string message)
{
	if (m_bConnected)
	{
		m_sMessage = message;
		_Send();
	}
	else
	{
		Init();
	}
	
}

void SerialPort::_Send()
{
	boost::asio::async_write(m_Port, boost::asio::buffer(m_sMessage), boost::bind(&SerialPort::_SendHandler,this, boost::asio::placeholders::error));
}

void SerialPort::_SendHandler(const boost::system::error_code& errorCode)
{
	if (errorCode == 0)
	{

	}
	else
	{
		EZLOGGERVLSTREAM(axter::log_rarely) << "Transmission of message failed!" << m_sPort <<  std::endl;
		_CloseConnection();
	}
}

///////////////////////////////////////////////////////////////
/// TCP Server
TCPServer::~TCPServer()
{ 
	m_Acceptor.close(); 
}

void TCPServer::Init()
{
	//m_IOService = NetworkConnection::m_IOService;
	_ListenConnection();
}

void TCPServer::_ListenConnection()
{
	auto connection = boost::shared_ptr<TCPConnection>(new TCPConnection(m_IOService));
	m_Acceptor.async_accept(connection->GetSocket(),
        boost::bind(&TCPServer::_HandleAccept, shared_from_this(), connection,
          boost::asio::placeholders::error));
}

void TCPServer::_HandleAccept(boost::shared_ptr<TCPConnection> connection,const boost::system::error_code& error)
{
	if (!error)
	{
		connection->Init();

		std::string s = connection->GetAddres();
		m_Connections[s] = connection;

		// create subscriber and add to statedistributor - need to find a more generic/cleaner way of doing this
		auto subscriber = boost::shared_ptr<NetworkSubscriber>(new NetworkSubscriber);
		subscriber->m_Connection = connection;
		subscriber->id = s;
		StateDistributor::GetInstance().AddSubscriber(subscriber);

		_ListenConnection();
	}
	else
	{
		// something went wrong in accepting the connection
		EZLOGGERVLSTREAM(axter::log_always) << "Connection attempt failed! " <<  std::endl;
	}
}

void TCPServer::Send(std::string description, std::string message)
{
	auto iterator = m_Connections.find(description);

	if (iterator != m_Connections.end())
		iterator->second->Send(message);
	
}

///////////////////////////////////////////////////////////////
/// TCP Connection

TCPConnection::~TCPConnection()
{
	_CloseConnection();
};

void TCPConnection::Init() 
{
	m_bConnected = true;
	m_sAddress = GetAddres();
}

void TCPConnection::Connect()
{

}

void TCPConnection::Close()
{

}

std::string TCPConnection::GetAddres()
{
	if (m_bConnected)
		return m_Socket.remote_endpoint().address().to_string();
	else return std::string();
}

void TCPConnection::Send(std::string message)
{
	if (m_bConnected)
	{
		m_sMessage = message;
		//_SendHeader();
		_Send();
	}
	else
	{
	
	}
	
}

void TCPConnection::_SendHeader()
{
	std::ostringstream ss;
	ss.width(4);
	ss.fill('0');
	ss <<  m_sMessage.length();
	std::string s(ss.str());
	const char*  c = s.c_str();
	boost::asio::async_write(m_Socket, boost::asio::buffer(c,4), boost::bind(&TCPConnection::_SendHeaderHandler,shared_from_this(), boost::asio::placeholders::error));
}

void TCPConnection::_SendHeaderHandler(const boost::system::error_code& errorCode)
{
	if (errorCode == 0)
	{
		_Send();
	}
	else
	{
		EZLOGGERVLSTREAM(axter::log_rarely) << "Transmission of message header failed! " << m_sAddress << ":" <<  m_sPort <<  std::endl;
		_CloseConnection();
	}
}

void TCPConnection::_Send()
{
	boost::asio::async_write(m_Socket, boost::asio::buffer(m_sMessage), boost::bind(&TCPConnection::_SendHandler,shared_from_this(), boost::asio::placeholders::error));
}



void TCPConnection::_SendHandler(const boost::system::error_code& errorCode)
{
	if (errorCode == 0)
	{

	}
	else
	{
		EZLOGGERVLSTREAM(axter::log_rarely) << "Transmission of message failed! " << m_sAddress << ":" <<  m_sPort <<  std::endl;
		_CloseConnection();
	}
}

void TCPConnection::_CloseConnection()
{
//	m_Socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send);
	m_Socket.close();
	m_bConnected = false;
	//m_bAddressResolved = false;	
	m_bConnecting = false;
}



void NetworkSubscriber::Send(std::string msg)
{
	if (m_Connection.get() != NULL)
		m_Connection->Send(msg);
}