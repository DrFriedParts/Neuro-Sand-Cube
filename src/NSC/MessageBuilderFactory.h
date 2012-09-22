#pragma once

#include "MessageBuilder.h"

#include <string>


class MessageBuilderFactory
{
public:
	MessageBuilderFactory(void); 
	~MessageBuilderFactory(void);

	static MessageBuilder& GetBuilder(std::string id);

	static SimpleBuilder simpleBuilder; 
	
};

