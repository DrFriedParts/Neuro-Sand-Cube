#ifndef __SHARED_STATE_DISTRIBUTOR_H__
#define __SHARED_STATE_DISTRIBUTOR_H__

#include "SharedState.h"

#include <vector>
#include <string>

#include <boost/shared_ptr.hpp>

/* This class is responsible for maintaining a list of states
 * that need to distributed, updating those states and generating
 * the messages which relay those states at the correct time, and
 * finally for dispatching those messages 

 */

struct SharedStateAttributes
{
	std::string id;
	// properties
	bool onlyOnChange;
	bool resetOnRestart;
	int interval;

	std::vector<std::string> consumers;
};

struct SharedStateDistribution
{
	SharedStateAttributes attributes;

	boost::shared_ptr<SharedState> data;
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

	void AddSharedState(boost::variant<int&, float&, bool&>, std::string id);

	void AddDistribution(SharedStateAttributes attributes);

	void Distribute(); // this should be called every frame.
	
private:
	boost::shared_ptr<SharedState> FindSharedState(std::string stateID);
	void SharedStateDistributor::AddDistributionMessage(boost::shared_ptr<SharedStateDistribution> distribution);
	void AddConsumerDistribution(std::string id, boost::shared_ptr<SharedStateDistribution> distribution);
	void AddConsumerDistribution(boost::shared_ptr<SharedStateDistribution> distribution);

	void ClearDistributionCache();

	void FlushDistribution();

	std::vector<boost::shared_ptr<SharedState> > sharedStates; // 

	std::vector<boost::shared_ptr<SharedStateDistribution> > distributions;

	std::vector<boost::shared_ptr<ConsumerDistributionCache> > consumers;

	int currentFrame;		// this is not the game frames, but the frames since the distributor was started. 
};

#endif
