#ifndef __SHARED_STATE_DISTRIBUTOR_H__
#define __SHARED_STATE_DISTRIBUTOR_H__

//#include "SharedState.h"

#include <vector>
#include <string>
#include <map>
#include <functional>

#include <boost/shared_ptr.hpp>
#include <boost/variant.hpp>
//#include <boost/function.hpp>


typedef boost::variant<int, float, bool> SharedState;
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

struct SharedStateDistribution
{
	SharedStateAttributes attributes;

	//boost::shared_ptr<SharedState> data;

	SharedStateFunctor valueFunction;

	SharedState value; 
	SharedState prevValue; 
	SharedState delta;
	// vars
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

	void AddSharedState(SharedState, std::string id);
	void AddSharedState(std::string id, SharedStateFunctor);

	bool AddDistribution(SharedStateAttributes attributes);

	void Distribute(); // this should be called every frame.
	
private:
	//boost::shared_ptr<SharedState> FindSharedState(std::string stateID);
	SharedStateFunctor* FindSharedStateFunctor(std::string stateID);
	void SharedStateDistributor::AddDistributionMessage(boost::shared_ptr<SharedStateDistribution> distribution);
	void AddConsumerDistribution(std::string id, boost::shared_ptr<SharedStateDistribution> distribution);
	void AddConsumerDistribution(boost::shared_ptr<SharedStateDistribution> distribution);

	void ClearDistributionCache();

	void FlushDistribution();

	//std::vector<boost::shared_ptr<SharedState> > sharedStates; // 

	std::vector<boost::shared_ptr<SharedStateDistribution> > distributions;

	std::vector<boost::shared_ptr<ConsumerDistributionCache> > consumers;

	std::map<std::string, SharedStateFunctor > sharedStates;

	int currentFrame;		// this is not the game frames, but the frames since the distributor was started. 
};

#endif
