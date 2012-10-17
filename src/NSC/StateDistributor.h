#ifndef __SHARED_STATE_DISTRIBUTOR_H__
#define __SHARED_STATE_DISTRIBUTOR_H__

#include "State.h"
#include "Connection.h"

#include <vector>
#include <string>
#include <map>
#include <functional>

#include <boost/shared_ptr.hpp>


struct StateSubscriber
{
	virtual void Send(std::string) =0;
	std::string id;
};

struct StateDistribution
{
	StateAttributes attributes;
	StateFunctor valueFunction;
	//Functor hasChanged

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
	static StateDistributor& GetInstance();
	~StateDistributor(void);

	void AddState(std::string id, StateFunctor);
	bool AddDistribution(StateAttributes attributes);
	void AddSubscriber(boost::shared_ptr<ConnectionInterface> subscriber);
	void Distribute(); // this should be called every frame.

	void ResetCounters();
	void ResetCounter(std::string id);
	
private:

	StateDistributor(void);

	void AddConsumerDistribution(boost::shared_ptr<StateDistribution> distribution);
	void FlushDistribution();

	//std::vector<boost::shared_ptr<State> > States; // 

	std::vector<boost::shared_ptr<StateDistribution> > distributions;
	std::vector<boost::shared_ptr<ConsumerDistributionCache> > consumers;
	std::vector<boost::shared_ptr<ConnectionInterface> > subscribers;
	std::map<std::string, boost::shared_ptr<ConsumerDistributionCache> > consumerMap;
	std::map<std::string, StateFunctor > states;

	int currentFrame;		// frames since the distributor was started. 
};




#endif
