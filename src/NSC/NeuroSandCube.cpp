#include "NeuroSandCube.h"
#include "EZLogger.h"
#include "ConfigReader.h"
#include "ConnectionMediator.h"
#include "CommandController.h"

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
	m_spCommandController = boost::shared_ptr<CommandController>(new CommandController());
	m_spConnectionMediator = boost::shared_ptr<ConnectionMediator>(new ConnectionMediator(m_spCommandController));
	m_spIOService = boost::shared_ptr<IOService>(new IOService());
	m_spServer = boost::shared_ptr<TCPServer>(new TCPServer(m_spIOService->m_IOService,12345,*m_spConnectionMediator));
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
	[player] () -> State
	{
		return State(player->levelRestart);

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

	distributor.AddState("teleport",
	[player] () -> State
	{
		return State(player->teleported);
	}
	);


	StateConfigReader configReader;

	configReader.ReadConfig(configFile);
	int i=0;
	boost::shared_ptr<StateAttributes> attributes = configReader.GetStateAttribute(i);

	std::set<std::string> serialPortSet;
	while (attributes.get() != NULL)
	{

		if (distributor.AddDistribution(*attributes))
		{
			for (unsigned int j=0; j < attributes->consumers.size(); ++j)
			{
				auto consumer = std::string(attributes->consumers[j]);
				std::istringstream oss(consumer);

				std::string sub = consumer.substr(0,3);
				if (sub.compare(std::string("COM")) == 0)
				{
					serialPortSet.insert(consumer);
				}

			}

		}
		else
		{

			EZLOGGERVLSTREAM(axter::log_always) << "Attempting to add distribution of unsuported state - " << attributes->id <<"!"<< std::endl;
		}
		attributes = configReader.GetStateAttribute(++i);

	}

	for (auto iterator = serialPortSet.begin(); iterator != serialPortSet.end(); iterator++)
	{
		/*distributor.AddSubscriber(*/CreateSerialPort(*iterator)/*)*/;
	}


	// setup command attributes

	m_spCommandController->AddCommand("restart_map",
		[player] (std::string target) { game::spawnplayer(player); });
	m_spCommandController->AddCommand("reset_counter",
		[player] (std::string target) { StateDistributor::GetInstance().ResetCounter(target); });

	

	i=0;
	auto commandAttribute = configReader.GetCommandAttribute(i);

	while (commandAttribute.get() != NULL)
	{

		m_spCommandController->AddConfiguration(*commandAttribute);
		commandAttribute = configReader.GetCommandAttribute(++i);
	}




}

void NeuroSandCube::Update()
{

	m_spCommandController->PollCommands();

	if (player->levelRestart)
		StateDistributor::GetInstance().ResetCounters();

	StateDistributor::GetInstance().Distribute();
	
	m_spIOService->Update();
	ResetFrame();

	
}

void NeuroSandCube::ResetFrame()
{
	
	player->levelRestart = false;
	player->teleported = false;
}


boost::shared_ptr<StateSubscriber>  NeuroSandCube::CreateSerialPort(std::string description)
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

		serialPort = boost::shared_ptr<SerialPort>(new SerialPort(m_spIOService->m_IOService,address,iBaudRate,eParity,iDataBits,fStopBits));
		/*subscriber->m_Connection = serialPort;
		subscriber->id = description;*/
		m_spConnectionMediator->AddConnection(description, serialPort);
	}

	return subscriber;
}
