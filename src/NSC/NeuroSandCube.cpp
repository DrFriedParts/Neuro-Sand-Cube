#include "NeuroSandCube.h"
#include "EZLogger.h"
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

	// flag captured
	// velocity

	// faced now with either extending States to have two modes of tracking - 

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
	while (attributes.get() != NULL)
	{
		
		
		if (distributor.AddDistribution(*attributes))
		{
			
			for (unsigned int j=0; j < attributes->consumers.size(); ++j)
			{
				auto consumer = std::string(attributes->consumers[j]);
				std::istringstream oss(consumer);

				if (!dispatchController.HasDispatcher(consumer))
				{
				
					std::string sub = consumer.substr(0,3);
					if (sub.compare(std::string("COM")) == 0)
					{
						auto dispatcher = boost::shared_ptr<MessageDispatcher>(new MessageDispatcher(consumer));
						dispatchController.AddDispatcher(consumer,dispatcher);
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


}

void NeuroSandCube::Update()
{
	distributor.Distribute();
	NetworkPort::Update();
}