#include "MessageBuilderFactory.h"
 
SimpleBuilder MessageBuilderFactory::m_simpleBuilder; 
JSONBuilder MessageBuilderFactory::m_JSONBuilder; 

// now this is what you call a useless factory.  It MAY be necessary
// in near future. if not remove TODO

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