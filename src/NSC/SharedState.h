#ifndef __SHARED_STATE_H__
#define __SHARED_STATE_H__

#include <string>
#include <boost/variant.hpp>

//using namespace std;

/* This is a simple class, which for now will act as the mechanism we will use to 
 * store and update "shared values", ie , the values we will be distributing via
 * the network, serial port, etc.
 *
 * It stores a value and keeps track of the previous value.
 * At the moment, this tracking is simply done by having a variable which is updated
 * every frame to the previous value.  I think this will be sufficient, using a 
 * observer pattern might be overkill, especially as we are integrating with cube.

*/

class SharedState_equals : public boost::static_visitor<bool>
{
public:
    template <typename T, typename U>
    bool operator()( const T &, const U & ) const
    {
        return false; // cannot compare different types
    }

    template <typename T>
    bool operator()( const T & lhs, const T & rhs ) const
    {
        return lhs == rhs;
    }

};


class SharedState
{
public:
	SharedState();
	~SharedState();

	void update();

	SharedState(boost::variant<int&, float&, bool&> value, std::string id);

	boost::variant<int&,float&, bool&> value; 
	boost::variant<int, float, bool> prevValue; 
	
	int numChanges;	// # times changed

	std::string id;
};

#endif