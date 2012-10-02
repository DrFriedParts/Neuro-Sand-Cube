#pragma once

#include "SharedStateDistributor.h"

// atm this class has two repsonsibilities - formatting the messages in JSON as well as formatting
// the messages in our NSC format (ie, it knows that we send timestamps and framestamps etc)
// perhaps seperate it from this?

class MessageBuilder
{
public:
    MessageBuilder(void);
	virtual ~MessageBuilder(void);

	//virtual void Add(SharedStateDistribution& distribution) = 0;
	//template <typename T>
	virtual void Add(std::string key, std::string value, bool str = false) = 0;
	virtual void Add(std::string key, std::string value, int numChanges) {};

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

	//virtual void Add(SharedStateDistribution& distribution);
	void Add(std::string key, std::string value, bool str = false){};
	virtual std::string Get(int framestamp);

	std::string message;
};


class JSONBuilder : public MessageBuilder
{
public:
	JSONBuilder();
	virtual ~JSONBuilder();

	//virtual void Add(SharedStateDistribution& distribution);
	virtual std::string Get(int framestamp);
	//template <typename T>
	void Add(std::string key, std::string value, bool str = false);
	void Add(std::string key, std::string value, int numChanges);
	std::string message;
};

