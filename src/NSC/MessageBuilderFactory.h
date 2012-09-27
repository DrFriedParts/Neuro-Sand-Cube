#pragma once

#include "MessageBuilder.h"

#include <string>


class MessageBuilderFactory
{
public:
	MessageBuilderFactory(void); 
	~MessageBuilderFactory(void);

	static MessageBuilder& GetBuilder(std::string id);

	static SimpleBuilder m_simpleBuilder; 
	static JSONBuilder m_JSONBuilder;
	
};

