#include "MessageBuilder.h"

#include <sstream>
#include <fstream>

#include "boost/date_time/posix_time/posix_time.hpp"
#include <time.h>



MessageBuilder::MessageBuilder(void)
{

}


MessageBuilder::~MessageBuilder(void)
{
}

void MessageBuilder::ConstructMessage(StateDistribution& distribution)
{

}


SimpleBuilder::SimpleBuilder()
{

}

SimpleBuilder::~SimpleBuilder()
{

}
/*
void SimpleBuilder::Add(StateDistribution& distribution)
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

}*/

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



JSONBuilder::JSONBuilder()
{

}

JSONBuilder::~JSONBuilder()
{

}

/*
void JSONBuilder::Add(StateDistribution& distribution)
{

	// im just hacking this in now - clean in next commit - change builder to take key and value
	std::ostringstream ss;
	
	if (distribution.attributes.delta)
		ss << distribution.delta;
	else
		ss << distribution.value;
	std::string s(ss.str());

	Add(distribution.attributes.id, s);	
}*/


void JSONBuilder::Add(std::string key, std::string value, bool str)
{
	std::ostringstream ss;
	if (message.compare("") != 0)
		ss << ", ";

	ss << "{ " << "\"id\": " << "\"" << key << "\", \"value\": " ;

	if (str)
		ss << "\"" << value << "\" ";
	else
		ss << value << " ";
	ss << " }";

	std::string s(ss.str());
	message.append(s);

}

std::string JSONBuilder::Get(int framestamp)
{
	if (message.compare("") == 0)
		return message;

	boost::posix_time::ptime t1(boost::posix_time::time_from_string("1970-01-01"));

	boost::posix_time::ptime t2(boost::posix_time::microsec_clock::local_time());

	boost::posix_time::time_period period(t1,t2);
	double t = period.length().total_milliseconds();

	std::ostringstream ss;
	
	//ss << t.time_of_day();

	time_t seconds = time(NULL);
	


	boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
	boost::posix_time::time_duration dur;

	dur = t2 - epoch; 

	long long tstamp = dur.total_milliseconds();

	//if (t < epoch) tstamp = -tstamp ; 
	ss << tstamp;

	std::string s(ss.str());

	



	Add("timestamp",s/*.substr(0,s.length() )*/,true); // leave out the last 3 - microseconds not necessary - i think ??
	ss = std::ostringstream();
	ss << framestamp;
	std::string s2(ss.str());
	Add("frame", s2);

	ss = std::ostringstream();
	ss << "[ " << message << " ]\r\n";

	std::string finalMessage(ss.str());

	
	message.clear();

	return finalMessage;
}

void JSONBuilder::Add(std::string key, std::string value, int numChanges)
{
	std::ostringstream ss;
	if (message.compare("") != 0)
		ss << " ,";
	ss << "{ " << "\"id\": " << "\"" << key << "\", \"value\": " ;


	ss << value << " ";
	ss << ", \"change_count\": " << numChanges << " } ";

	std::string s(ss.str());
	message.append(s);

}