#include "NeuroSandCube.h"
#include "EZLogger.h"
#include "ConfigReader.h"
#include "MessageDispatchController.h"
#include "MessageDispatcher.h"

#include <sstream>
#include <set>
#include <boost/foreach.hpp>



NeuroSandCube::NeuroSandCube(void)
{
	StateDistributor::GetInstance();

	
}


NeuroSandCube::~NeuroSandCube(void)
{
}


void NeuroSandCube::Initialize(fpsent* player)
{

	m_spServer = boost::shared_ptr<TCPServer>(new TCPServer(12345));
	m_spServer->Init();
	this->player = player;

	std::string configFile = "data/NSC/nsc_config.json";

	// others possibilites:

	// flag captured
	// velocity

	// faced now with either extending States to have two modes of tracking - 
	StateDistributor& distributor = StateDistributor::GetInstance();

	distributor.AddState("player_x",
	[player] ()
	{
		return State(player->newpos.x);
	}
	);

	distributor.AddState("player_y",
	[player] ()
	{
		return State(player->newpos.y);
	}
	);

	distributor.AddState("player_left_click",
	[player] ()
	{
		return State(player->attacking);
	}
	);

	distributor.AddState("level_restart",
	[player] ()
	{
		return State(player->respawned);
	}
	);

	distributor.AddState("player_angle",
	[player] ()
	{
		return State(player->yaw);
	}
	);

	distributor.AddState("distance_traveled",
	[player] ()
	{
		return State( player->newpos.dist(player->startingPosition));
	}
	);

	StateConfigReader configReader;
	

	configReader.ReadConfig(configFile);
	int i=0;
	boost::shared_ptr<StateAttributes> attributes = configReader.Get(i);

	MessageDispatchController& dispatchController = MessageDispatchController::GetInstance();
	std::set<std::string> serialPortSet;
	while (attributes.get() != NULL)
	{

		if (distributor.AddDistribution(*attributes))
		{
			for (unsigned int j=0; j < attributes->consumers.size(); ++j)
			{
				auto consumer = std::string(attributes->consumers[j]);
				std::istringstream oss(consumer);

				//if (!dispatchController.HasDispatcher(consumer))
				{
				
					std::string sub = consumer.substr(0,3);
					if (sub.compare(std::string("COM")) == 0)
					{
						//auto dispatcher = boost::shared_ptr<MessageDispatcher>(new MessageDispatcher(consumer));
						//dispatchController.AddDispatcher(consumer,dispatcher);

						//distributor.AddSubscriber(CreateSerialPortSubscriber(consumer));
						serialPortSet.insert(consumer);
					}
				}
			}

		}
		else
		{
			EZLOGGERVLSTREAM(axter::log_always) << "Attempting to add distribution of unsuported state - " << attributes->id <<"!"<< std::endl;
		}
		attributes = configReader.Get(++i);

	}

	for (auto iterator = serialPortSet.begin(); iterator != serialPortSet.end(); iterator++)
	{
		distributor.AddSubscriber(CreateSerialPortSubscriber(*iterator));
	}


}

void NeuroSandCube::Update()
{
	StateDistributor::GetInstance().Distribute();
	NetworkConnection::Update();
}

boost::shared_ptr<StateSubscriber>  NeuroSandCube::CreateSerialPortSubscriber(std::string description)
{
	// i have moved this here temporarily since i dont know where to handle the creation right now.

	// instantiates appropriate network port based on device description
	// if its a com address -> serial port

	boost::shared_ptr<NetworkConnection> serialPort;
	auto subscriber = boost::shared_ptr<NetworkSubscriber>(new NetworkSubscriber);

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
			EZLOGGERVLSTREAM(axter::log_always) << "Failed to initialize serial port - " << description <<". Incorrect format specified!"<< std::endl;
		}

		serialPort = boost::shared_ptr<SerialPort>(new SerialPort(address,iBaudRate,eParity,iDataBits,fStopBits));
		subscriber->m_Connection = serialPort;
		subscriber->id = description;
	}

	return subscriber;
}
