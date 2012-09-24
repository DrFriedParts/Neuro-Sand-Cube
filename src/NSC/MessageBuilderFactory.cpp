#include "MessageBuilderFactory.h"
 
SimpleBuilder MessageBuilderFactory::m_simpleBuilder; 
JSONBuilder MessageBuilderFactory::m_JSONBuilder; 


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
	//return simpleBuilder;
	return m_JSONBuilder;
}