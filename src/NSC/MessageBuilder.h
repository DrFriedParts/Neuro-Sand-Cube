#pragma once

#include "SharedStateDistributor.h"

// for now this will construct messages directly 
// shoudl perhaps make this more flexible, and abstract away
// the message construction so it can be generated in any form
// perhaps using a builder

// make proper builder todo	

class MessageBuilder
{
public:
    MessageBuilder(void);
	virtual ~MessageBuilder(void);

	virtual void AddMessage(SharedStateDistribution& distribution) = 0;
	virtual std::string GetMessage(int timestamp) = 0;

private:

	void ConstructMessage(SharedStateDistribution& distribution);

};

class SimpleBuilder : public MessageBuilder
{
public:
	SimpleBuilder();
	virtual ~SimpleBuilder();

	virtual void AddMessage(SharedStateDistribution& distribution);
	virtual std::string GetMessage(int framestamp);

	std::string message;
};

