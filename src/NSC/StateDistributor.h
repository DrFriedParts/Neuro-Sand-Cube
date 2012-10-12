#ifndef __SHARED_STATE_DISTRIBUTOR_H__
#define __SHARED_STATE_DISTRIBUTOR_H__

#include "State.h"

#include <vector>
#include <string>
#include <map>
#include <functional>

#include <boost/shared_ptr.hpp>

struct StateDistribution
{
	StateAttributes attributes;

	StateFunctor valueFunction;

	State value; 
	State prevValue; 
	State delta;
	
	int numChanges; // number of time value has changed
	int framesPassedSinceDistribution;

	bool toBeDistributed;

	
};

struct ConsumerDistributionCache
{
	std::string id;
	std::vector<boost::shared_ptr<StateDistribution> > stateCache;

};



class StateDistributor
{
public:
	StateDistributor(void);
	~StateDistributor(void);

	void AddState(std::string id, StateFunctor);
	bool AddDistribution(StateAttributes attributes);
	void Distribute(); // this should be called every frame.
	
private:

	void AddConsumerDistribution(boost::shared_ptr<StateDistribution> distribution);
	void FlushDistribution();

	//std::vector<boost::shared_ptr<State> > States; // 

	std::vector<boost::shared_ptr<StateDistribution> > distributions;
	std::vector<boost::shared_ptr<ConsumerDistributionCache> > consumers;
	std::map<std::string, StateFunctor > states;

	int currentFrame;		// frames since the distributor was started. 
};

#endif
