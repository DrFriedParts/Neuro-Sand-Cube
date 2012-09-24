#include "NeuroSandCube.h"

#include "ConfigReader.h"
#include "MessageDispatchController.h"
#include "MessageDispatcher.h"

#include <sstream>
#include <boost/foreach.hpp>


NeuroSandCube::NeuroSandCube(void)
{
}


NeuroSandCube::~NeuroSandCube(void)
{
}


void NeuroSandCube::Initialize(fpsent* player)
{
	this->player = player;

	std::string configFile = "data/NSC/nsc_config.json";

	// others possibilites:
	// distance
	// anggle
	// flag captured
	// velocity

	// faced now with either extending sharedstates to have two modes of tracking - 

	distributor.AddSharedState("player_x",
	[player] ()
	{
		return SharedState(player->newpos.x);
	}
	);

	distributor.AddSharedState("player_y",
	[player] ()
	{
		return SharedState(player->newpos.y);
	}
	);

	distributor.AddSharedState("player_left_click",
	[player] ()
	{
		return SharedState(player->attacking);
	}
	);

	distributor.AddSharedState("level_restart",
	[player] ()
	{
		return SharedState(player->respawned);
	}
	);

	distributor.AddSharedState("player_angle",
	[player] ()
	{
		return SharedState(player->yaw);
	}
	);

	distributor.AddSharedState("distance_traveled",
	[player] ()
	{
		return SharedState( player->newpos.dist(player->startingPosition));
	}
	);


	
	/*distributor.AddSharedState(boost::variant<int&,float&, bool&>(player->newpos.x),"player_x");
	distributor.AddSharedState(boost::variant<int&,float&, bool&>(player->newpos.y),"player_y");
	distributor.AddSharedState(boost::variant<int&,float&, bool&>(player->attacking),"player_left_click");
	distributor.AddSharedState(boost::variant<int&, float&, bool&>(player->respawned),"level_restart");*/

	SharedStateConfigReader configReader;
	

	configReader.ReadConfig(configFile);
	int i=0;
	boost::shared_ptr<SharedStateAttributes> attributes = configReader.Get(i);

	MessageDispatchController& dispatchController = MessageDispatchController::GetInstance();
	while (attributes.get() != NULL)
	{
		
		
		if (distributor.AddDistribution(*attributes))
		{
			// to clean up, create TCPDispatcher via factory or other proxy
			for (unsigned int j=0; j < attributes->consumers.size(); ++j)
			{
				auto consumer = std::string(attributes->consumers[j]);
				std::istringstream oss(consumer);
				std::string ip, port;
				getline(oss , ip, ':'); 
				getline(oss , port, ':'); 

				if (!dispatchController.HasDispatcher(consumer))
				{
					auto dispatcher = boost::shared_ptr<MessageDispatcher>(new MessageDispatcher(consumer));
					dispatchController.AddDispatcher(consumer,dispatcher);
				}
			}
		
			
		}
		else
		{
			// log
		}
		attributes = configReader.Get(++i);

	}


}

void NeuroSandCube::Update()
{
	distributor.Distribute();
	NetworkPort::Update();
}