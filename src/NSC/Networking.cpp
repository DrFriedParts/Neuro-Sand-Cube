#include "Globals.h"

#include "Networking.h"
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../ezlogger/ezlogger_headers.hpp"

#include "ConnectionMediator.h"


std::string NetworkConnection::Receive()
{
	_Receive(); 

	std::string msg;
	if (receiveQueue.size() > 0) 
	{
		msg = receiveQueue.front();
		receiveQueue.pop(); 
	}
	return msg;

};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
// Serial port
////


SerialPort::SerialPort(boost::shared_ptr<boost::asio::io_service> ioservice, std::string port, int iBaudRate, SerialPort_Parity eParity, int iDataBits, float fStopBits ) : 
	m_spIOService(ioservice),
	m_sPort(port),
	m_iBaudRate(iBaudRate),
	m_eParity(eParity),
	m_iDataBits(iDataBits),
	m_fStopBits(fStopBits)
{
	m_Port = boost::shared_ptr<boost::asio::serial_port>(new boost::asio::serial_port(*m_spIOService));
	 
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
		m_Port->open(m_sPort);
	
		if (!m_Port->is_open())
		{
			EZLOGGERVLSTREAM(axter::log_rarely) << "Failed to open serial port - " << m_sPort <<"!"<< std::endl;
		}

		m_bConnected = true;

		m_Port->set_option(boost::asio::serial_port::baud_rate(m_iBaudRate));
		if (m_eParity == SP_PARITY_NONE)
			m_Port->set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none));
		else if (m_eParity == SP_PARITY_ODD)
			m_Port->set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::odd));
		else
			m_Port->set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::even));
	
		if (m_fStopBits == 1.0f)
			m_Port->set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));
		else if (m_fStopBits < 2.0f)
			m_Port->set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::onepointfive));
		else
			m_Port->set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::two));

		m_Port->set_option(boost::asio::serial_port::character_size(m_iDataBits));
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
	m_spIOService->post(boost::bind(&SerialPort::_CloseConnection, this));
}

void SerialPort::_CloseConnection()
{
	//m_Port.shutdown();
	m_Port->close();
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
	boost::asio::async_write(*m_Port, boost::asio::buffer(m_sMessage), boost::bind(&SerialPort::_SendHandler,shared_from_this(), boost::asio::placeholders::error));
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

void SerialPort::_Receive()
{
	if (m_bConnected)
	{
		boost::asio::async_read_until(*m_Port, buffer, '\n',
			boost::bind(&SerialPort::_ReceiveHandler, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		Init();
	}

}

void SerialPort::_ReceiveHandler(const boost::system::error_code& error_code,size_t bytes_transferred)
{
	if (error_code ==0 )
	{
		std::istream is(&buffer);
		std::string s,t;
		//std::getline(is,s);
		is >> s;
		receiveQueue.push(s);
	}
	else
	{
		EZLOGGERVLSTREAM(axter::log_always) << "Error receiving command! " <<  std::endl;
	}
}

///////////////////////////////////////////////////////////////
/// TCP Server
TCPServer::~TCPServer()
{ 
	//m_Acceptor.close(); 
}

void TCPServer::Init()
{
	_ListenConnection();
}

void TCPServer::_ListenConnection()
{
	auto connection = boost::shared_ptr<TCPConnection>(new TCPConnection(m_spIOService));
	m_Acceptor->async_accept(connection->GetSocket(),
        boost::bind(&TCPServer::_HandleAccept,shared_from_this(), connection,
          boost::asio::placeholders::error));
}

void TCPServer::_HandleAccept(boost::shared_ptr<TCPConnection> connection,const boost::system::error_code& error)
{
	if (!error)
	{
		connection->Init();

		std::string s = connection->GetAddres();
		m_Connections[s] = connection;

		m_Mediator.AddConnection(s,connection);

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

void TCPConnection::_Receive()
{
	if (m_bConnected)
	{
		/*m_Port->async_read_some()*/
		boost::asio::async_read_until(m_Socket, buffer, "}",
			boost::bind(&TCPConnection::_ReceiveHandler, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		Init();
	}

}

void TCPConnection::_ReceiveHandler(const boost::system::error_code& error_code,size_t bytes_transferred)
{
	if (error_code ==0 )
	{
		std::istream is(&buffer);
		std::string s,t;
		std::getline(is,s);
		receiveQueue.push(s);
	}
	else
	{
		EZLOGGERVLSTREAM(axter::log_always) << "Error receiving command! " <<  std::endl;
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