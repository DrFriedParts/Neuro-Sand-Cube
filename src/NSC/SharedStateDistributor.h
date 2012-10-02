#ifndef __SHARED_STATE_DISTRIBUTOR_H__
#define __SHARED_STATE_DISTRIBUTOR_H__

//#include "SharedState.h"

#include <vector>
#include <string>
#include <map>
#include <functional>

#include <boost/shared_ptr.hpp>
#include <boost/variant.hpp>

#include "Globals.h"


typedef boost::variant<int, float, bool> SharedState;				// shared states can be int, float, or bool. others can be added but this is fine for now
typedef std::function<SharedState ()> SharedStateFunctor;


/* This class is responsible for maintaining a list of states
 * that need to distributed, updating those states and generating
 * the messages which relay those states at the correct time, and
 * finally for dispatching those messages 

 */

struct SharedStateAttributes
{
	SharedStateAttributes() :
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



class SharedState_equals : public boost::static_visitor<bool>
{
public:
    template <typename T, typename U>
    bool operator()( const T &lhs, const U &rhs ) const
    {
        return lhs == rhs; // cannot compare different types
    }

    template <typename T>
    bool operator()( const T & lhs, const T & rhs ) const
    {
        return (abs(lhs - rhs) < (EPSILON));
    }

};

class SharedState_difference : public boost::static_visitor< float > //hax
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

class SharedState_tostring : public boost::static_visitor< std::string > //hax
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

struct SharedStateDistribution
{
	SharedStateAttributes attributes;

	SharedStateFunctor valueFunction;

	SharedState value; 
	SharedState prevValue; 
	SharedState delta;
	
	int numChanges; // number of time value has changed
	int framesPassedSinceDistribution;

	bool toBeDistributed;

	
};

struct ConsumerDistributionCache
{
	std::string id;
	std::vector<boost::shared_ptr<SharedStateDistribution> > sharedStateCache;

};



class SharedStateDistributor
{
public:
	SharedStateDistributor(void);
	~SharedStateDistributor(void);

	void AddSharedState(std::string id, SharedStateFunctor);
	bool AddDistribution(SharedStateAttributes attributes);
	void Distribute(); // this should be called every frame.

	void LevelReset();
	
private:

	void AddConsumerDistribution(boost::shared_ptr<SharedStateDistribution> distribution);
	void FlushDistribution();

	//std::vector<boost::shared_ptr<SharedState> > sharedStates; // 

	std::vector<boost::shared_ptr<SharedStateDistribution> > distributions;
	std::vector<boost::shared_ptr<ConsumerDistributionCache> > consumers;
	std::map<std::string, SharedStateFunctor > sharedStates;

	int currentFrame;		// frames since the distributor was started. 
};

#endif
