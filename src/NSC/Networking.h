#ifndef __NETWORKING_H__
#define __NETWORKING_H__

#include <string>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

// basic networking classes used for transmission of plain text messages over tcp

class TCPClient
{
public:
	TCPClient(std::string ip, std::string port);
	~TCPClient();

	void Init(); // resolve address & connect
	void Connect();
	void Close();
	void Send(std::string);

	static void Update() { m_IOService.poll(); m_IOService.reset(); }// this will probably be moved somehwere to some network manager or something - just getting by

private:
	void _ConnectHandler(const boost::system::error_code& errorCode);

	void _SendHeader();
	void _SendHeaderHandler(const boost::system::error_code& errorCode);
	void _Send();
	void _SendHandler(const boost::system::error_code& errorCode);
	void _CloseConnection();

	void _ResolveAddress();
	void _ResolveAddressAsync();
	void _ResolveAddressHandler(const boost::system::error_code& err, tcp::resolver::iterator endpoint_iterator);	


	static boost::asio::io_service m_IOService;
	tcp::socket m_Socket;

	tcp::resolver::iterator m_EndPointIteratorBegin;
	tcp::resolver::iterator m_EndPointIterator;
	
	std::string m_sAddress;
	std::string m_sPort;
	
	bool m_bConnected;
	bool m_bConnecting;
	bool m_bAddressResolved;
	std::string m_sMessage;

};


// implement if needed
class TCPServer
{
	TCPServer();
	~TCPServer();

private:

	boost::asio::io_service io_service;

};


#endif