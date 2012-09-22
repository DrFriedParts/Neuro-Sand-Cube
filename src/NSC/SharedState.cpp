#include "SharedState.h"

#include <typeinfo>


SharedState::SharedState(boost::variant<int&,float&, bool&>  v, std::string _id) : value(v), id(_id)
{
	prevValue = value;
	numChanges = 0;
}

SharedState::~SharedState(void)
{
}

// at the moment, this wont do much
// perhaps later on, instead of using
// pointers to data - we can use lambda functions
// to provide the updated values. this will 
// require a bit more work on the user side.
// so for now, going as simple as possible

void SharedState::update()
{
	
}
