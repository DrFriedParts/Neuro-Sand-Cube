#include "MessageBuilder.h"

#include "MessageDispatchController.h"

#include <sstream>
#include <fstream>

#include "boost/date_time/posix_time/posix_time.hpp"



MessageBuilder::MessageBuilder(void)
{

}


MessageBuilder::~MessageBuilder(void)
{
}

void MessageBuilder::ConstructMessage(SharedStateDistribution& distribution)
{

}


SimpleBuilder::SimpleBuilder()
{

}

SimpleBuilder::~SimpleBuilder()
{

}

void SimpleBuilder::Add(SharedStateDistribution& distribution)
{

	// testing code
	std::ostringstream ss;
	ss << distribution.attributes.id << ": " ;
	if (distribution.attributes.delta)
		ss << distribution.delta << " ";
	else
		ss << distribution.value << " ";
	std::string s(ss.str());
	message.append(s);

}

std::string SimpleBuilder::Get(int framestamp)
{
	// testing code
	if (message.compare("") == 0)
		return message;
	boost::posix_time::ptime t(boost::posix_time::microsec_clock::local_time());
	std::ostringstream ss;
	ss << t.time_of_day() << " #" << framestamp << " "<< message;
	std::string finalMessage(ss.str());

	
	message.clear();

	return finalMessage;
}