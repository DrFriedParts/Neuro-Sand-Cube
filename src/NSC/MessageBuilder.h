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

	virtual void Add(SharedStateDistribution& distribution) = 0;
	//virtual void Add(string key, string value
	virtual std::string Get(int timestamp) = 0;

private:

	void ConstructMessage(SharedStateDistribution& distribution);

};

class SimpleBuilder : public MessageBuilder
{
public:
	SimpleBuilder();
	virtual ~SimpleBuilder();

	virtual void Add(SharedStateDistribution& distribution);
	virtual std::string Get(int framestamp);

	std::string message;
};


class JSONBuilder : public MessageBuilder
{
public:
	JSONBuilder();
	virtual ~JSONBuilder();

	virtual void Add(SharedStateDistribution& distribution);
	virtual std::string Get(int framestamp);
	void Add(std::string key, std::string value, bool str = false);
	std::string message;
};

