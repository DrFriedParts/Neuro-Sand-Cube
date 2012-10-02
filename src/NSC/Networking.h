#ifndef __NETWORKING_H__
#define __NETWORKING_H__

#include <string>

#include <map>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

using boost::asio::ip::tcp;

class IOService
{
public:
	IOService()  
	{
		
	};
	~IOService() { };

	static void Update() { m_IOService.poll(); m_IOService.reset(); }// this will probably be moved somewhere to some network manager or something - just getting by

	static boost::asio::io_service m_IOService;
private:

};



class NetworkPort
{
public:
	NetworkPort();

	~NetworkPort() { };

	virtual void Init() = 0; 
	virtual void Connect()= 0;
	virtual void Close()= 0;
	virtual void Send(std::string)= 0;

	//static boost::asio::io_service& GetIOService();


	//static void Update() { m_IOService.poll(); m_IOService.reset(); }// this will probably be moved somehwere to some network manager or something - just getting by
	//static boost::asio::io_service& m_IOService;
protected:
	
	IOService m_IOService;
	
	bool m_bConnected;
	bool m_bConnecting;

	
	std::string m_sMessage;

};

// basic networking classes used for transmission of plain text messages over tcp
/*
class TCPClient : public NetworkPort
{
public:
	TCPClient(std::string ip, std::string port);
	~TCPClient();

	void Init(); // resolve address & connect
	void Connect();
	void Close();
	void Send(std::string);

protected:
	void _ConnectHandler(const boost::system::error_code& errorCode);

	void _SendHeader();
	void _SendHeaderHandler(const boost::system::error_code& errorCode);
	void _Send();
	void _SendHandler(const boost::system::error_code& errorCode);
	void _CloseConnection();

	void _ResolveAddress();
	void _ResolveAddressAsync();
	void _ResolveAddressHandler(const boost::system::error_code& err, tcp::resolver::iterator endpoint_iterator);	


	tcp::socket m_Socket;

	tcp::resolver::iterator m_EndPointIteratorBegin;
	tcp::resolver::iterator m_EndPointIterator;
	
	std::string m_sAddress;
	std::string m_sPort;
	
	bool m_bAddressResolved;

};*/

enum SerialPort_Parity
{
	SP_PARITY_NONE,
	SP_PARITY_ODD,
	SP_PARITY_EVEN
};

class SerialPort: public NetworkPort
{
public:
	SerialPort(std::string port, int iBaudRate, SerialPort_Parity eParity, int iDataBits, float fStopBits );
	~SerialPort();

	void Init(); 
	void Connect();
	void Close();
	void Send(std::string);

protected:

	void _Send();
	void _SendHandler(const boost::system::error_code& errorCode);
	void _CloseConnection();

	boost::asio::serial_port m_Port;
	
	std::string m_sPort;

	int m_iBaudRate;
	SerialPort_Parity m_eParity;
	float m_fStopBits;
	int m_iDataBits;

};

class TCPConnection : public NetworkPort, public boost::enable_shared_from_this<TCPConnection>
{
public:
	TCPConnection(/*boost::asio::io_service& IOService*/);

	~TCPConnection() { };

	virtual void Init() ; 
	virtual void Connect();
	virtual void Close();
	virtual void Send(std::string);

	tcp::socket& GetSocket() { return m_Socket; };
	std::string GetAddres();

protected:
	
	void _SendHeader();
	void _SendHeaderHandler(const boost::system::error_code& errorCode);
	void _Send();
	void _SendHandler(const boost::system::error_code& errorCode);
	void _CloseConnection();
	
	bool m_bConnected;
	bool m_bConnecting;

	tcp::socket m_Socket;

	boost::array<char, 128> m_Buffer;

	char headerBuffer[5 + 512];
	int messageSize;

	tcp::resolver::iterator m_EndPointIteratorBegin;
	tcp::resolver::iterator m_EndPointIterator;
	
	std::string m_sAddress;
	std::string m_sPort;

	//boost::asio::io_service& m_IOService;

};

// implement if needed
class TCPServer 
{
public:
	

	void Init();
		
	~TCPServer() { }

	void Send(std::string description, std::string message);

	static TCPServer& GetInstance();
	//static void Update() { m_IOService.poll(); m_IOService.reset(); }// this will probably be moved somehwere to some network manager or something - just getting by

private:
	TCPServer(/*boost::asio::io_service& IOService,*/ int port);
	void _HandleAccept(boost::shared_ptr<TCPConnection> connection, const boost::system::error_code& error);

	IOService m_IOService;

	std::map<std::string, boost::shared_ptr<TCPConnection> > m_Connections;

	//boost::asio::io_service& m_IOService;
	tcp::acceptor m_Acceptor;

};


#endif