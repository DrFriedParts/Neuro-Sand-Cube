#include "MessageBuilderFactory.h"
 
SimpleBuilder MessageBuilderFactory::simpleBuilder; 


MessageBuilderFactory::MessageBuilderFactory(void)
{
}


MessageBuilderFactory::~MessageBuilderFactory(void)
{
}


MessageBuilder& MessageBuilderFactory::GetBuilder(std::string id)
{
	// if (id.compare("SERIAL") for eg
	//return networkMessenger;
	return simpleBuilder;
}