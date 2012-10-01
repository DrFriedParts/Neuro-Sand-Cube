#include "MessageDispatcher.h"
#include "Logger.h"

#include <sstream>
#include <algorithm>
#include <ctype.h>
#include <boost/foreach.hpp>



MessageDispatcher::MessageDispatcher(std::string description)
{
	// instantiates appropriate network port based on device description
	// if its a com address -> serial port

	Logger& logger = Logger::GetInstance();

	

	std::transform(description.begin(), description.end(),description.begin(), ::toupper);
	std::istringstream oss(description);
	std::string address, rest;
	getline(oss , address, ':'); 
	getline(oss , rest, ':'); 

	std::string sub = address.substr(0,3);
	
	if (sub.compare(std::string("COM")) == 0)
	{
		// init com
		
		int iBaudRate;
		SerialPort_Parity eParity;
		int iDataBits ;
		float fStopBits;


		std::string sBaudRate, sDataBits;;
		// yuck
		for (unsigned int i = 0; i < rest.size(); ++i)
		{
			char ch = rest[i];

			if (!isdigit(ch))
			{
				// done with baud rate - convert
				iBaudRate = atoi(sBaudRate.c_str());

				if (ch == 'N')
					eParity = SP_PARITY_NONE;
				else if (ch == 'O')
					eParity = SP_PARITY_ODD;
				else
					eParity = SP_PARITY_EVEN;

				char  sDataBits[2]  = { rest[i+1], '\0' };
				iDataBits = atoi(sDataBits); 
				const char* r = rest.c_str();
				fStopBits = atof(&(r[i+2]));
				break;
			}
			else
			{
				sBaudRate.insert(sBaudRate.size(),1,ch);
			}
		}


		if (iBaudRate == 0 || iDataBits == 0 || fStopBits == 0.0f)
		{
			//FAIL
			logger.Log(description);
			logger.Log(": Failed to initialize serial Port. Check parameters");
		}

		m_spPort = boost::shared_ptr<SerialPort>(new SerialPort(address,iBaudRate,eParity,iDataBits,fStopBits));
	}
	else
	{
		//network socket
//		m_spPort = boost::shared_ptr<TCPClient>(new TCPClient(address,rest));
	}

	
}


MessageDispatcher::~MessageDispatcher(void)
{
	m_spPort->Close();
}

void MessageDispatcher::Send(std::string msg)  
{
	m_spPort->Send(msg);
}

