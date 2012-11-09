#pragma once

#include "StateDistributor.h"
#include "game.h"

//forward decl
class TCPServer;
class IOService;
class ConnectionMediator;
class CommandController;

class NeuroSandCube
{
public:
	NeuroSandCube(void);
	~NeuroSandCube(void);

	void Initialize(fpsent* player);

	boost::shared_ptr<StateSubscriber> CreateSerialPort(std::string);
	void Update();

			

private:
	void ResetLevel();
	void ResetFrame();

	//StateDistributor& distributor;
	fpsent* player;

	boost::shared_ptr<ConnectionMediator> m_spConnectionMediator;
	boost::shared_ptr<TCPServer> m_spServer; 
	boost::shared_ptr<IOService> m_spIOService;
	boost::shared_ptr<CommandController> m_spCommandController;
};

