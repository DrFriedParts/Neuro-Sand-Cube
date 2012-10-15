#ifndef __STATE_H__
#define __STATE_H__

#include <boost/shared_ptr.hpp>
#include <boost/variant.hpp>
#include <functional>
#include <vector>
#include <string>

typedef boost::variant<int, float, bool> State;				// shared states can be int, float, or bool. others can be added but this is fine for now
typedef std::function<State ()> StateFunctor;


/* This class is responsible for maintaining a list of states
 * that need to distributed, updating those states and generating
 * the messages which relay those states at the correct time, and
 * finally for dispatching those messages 

 */

struct StateAttributes
{
	StateAttributes() :
		delta(false),
		onlyOnChange(false),
		resetOnRestart(false),
		interval(1) {};

	std::string id;
	// properties
	bool delta;
	bool onlyOnChange;
	bool resetOnRestart;
	int interval;

	std::vector<std::string> consumers;
};

class State_equals : public boost::static_visitor<bool>
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

class State_difference : public boost::static_visitor< float > //hax
{
public:

	template <typename T, typename U>
    T operator()( const T & lhs, const U & rhs) const
    {
       return lhs - rhs;// hope to gawd no non numeric types are used mmkay
    }

    template <typename T>
    T operator()( const T & lhs, const T & rhs ) const
    {
        return lhs - rhs;
    }

};

class State_tostring : public boost::static_visitor< std::string > //hax
{
public:

	template <typename T>
    std::string operator()( const T & t) const
    {
		std::ostringstream ss;
		ss << t;
		return std::string(ss.str());
    }

};

#endif