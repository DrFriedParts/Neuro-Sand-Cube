#pragma once

#include "StateDistributor.h"
#include "game.h"

//forward decl
class TCPServer;
class IOService;

class NeuroSandCube
{
public:
	NeuroSandCube(void);
	~NeuroSandCube(void);

	void Initialize(fpsent* player);

	boost::shared_ptr<StateSubscriber> CreateSerialPortSubscriber(std::string);
	void Update();

	void ResetFrame();		

private:

	//StateDistributor& distributor;
	fpsent* player;

	boost::shared_ptr<TCPServer> m_spServer; 
	boost::shared_ptr<IOService> m_spIOService;
};

