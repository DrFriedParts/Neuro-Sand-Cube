#include "SharedStateDistributor.h"

#include <assert.h>

#include "MessageBuilderFactory.h"
#include "MessageDispatchController.h"


SharedStateDistributor::SharedStateDistributor(void) : currentFrame(0)
{
	
}


SharedStateDistributor::~SharedStateDistributor(void)
{
	sharedStates.empty();  
	distributions.empty();  
}


void SharedStateDistributor::AddSharedState(std::string id, SharedStateFunctor functor)
{
	sharedStates[id] = functor;
	
}


bool SharedStateDistributor::AddDistribution(SharedStateAttributes attributes)
{

	boost::shared_ptr<SharedStateDistribution> distribution = boost::shared_ptr<SharedStateDistribution>(new SharedStateDistribution());
	
	distribution->attributes = attributes;

	distribution->framesPassedSinceDistribution = 0;
	distribution->numChanges = 0;
	distribution->toBeDistributed = false;

	// add shared state

	auto iterator = sharedStates.find(attributes.id);
		
	if (iterator != sharedStates.end())
		distribution->valueFunction = iterator->second;
	else
		return false;

	distributions.push_back(distribution);

	AddConsumerDistribution(distribution);
	
	return true;

}




void SharedStateDistributor::AddConsumerDistribution(boost::shared_ptr<SharedStateDistribution> distribution)
{
	// the config file specifies the consumers per shared state.  It is more efficient
	// to send all the messages for a consumer per frame in one message though, 
	// so here we simply invert this and add the states that need to be distributed
	// for each consumer
	for (unsigned int j = 0; j < distribution->attributes.consumers.size(); ++j)
	{
		bool consumerExists = false;
		for (unsigned int k = 0; k < consumers.size(); ++k)
		{
			boost::shared_ptr<ConsumerDistributionCache> consumer = consumers[k];
			if (consumer->id.compare(distribution->attributes.consumers[j]) == 0) // match
			{
				// already exists, append to cache
				consumer->sharedStateCache.push_back(distribution);
				consumerExists = true;
			}
		}
		if (!consumerExists)
		{
			boost::shared_ptr<ConsumerDistributionCache> consumer = boost::shared_ptr<ConsumerDistributionCache>(new ConsumerDistributionCache());
			consumer->id = distribution->attributes.consumers[j];
			consumer->sharedStateCache.push_back(distribution);

			consumers.push_back(consumer);
		}
	}

}

void SharedStateDistributor::Distribute()
{
	for (unsigned int i =0; i < distributions.size(); ++i)
	{
		boost::shared_ptr<SharedStateDistribution> distribution = distributions[i];
		distribution->value = distribution->valueFunction();
		bool hasChanged = !(boost::apply_visitor(SharedState_equals(), distribution->value , distribution->prevValue));
		
		if ((distribution->attributes.onlyOnChange && hasChanged) || (distribution->framesPassedSinceDistribution >= distribution->attributes.interval && !distribution->attributes.onlyOnChange))
		{
			// distribute
			distribution->framesPassedSinceDistribution = 0;
			distribution->toBeDistributed = true;

			distribution->delta = boost::apply_visitor(SharedState_difference(), distribution->value , distribution->prevValue);
			distribution->prevValue = distribution->value;

		}
		else
		{
			distribution->toBeDistributed = false;
		}

		if (hasChanged)
			++distribution->numChanges;

		++distribution->framesPassedSinceDistribution;
	}

	FlushDistribution();

	++currentFrame;
}


// this will dispatch the messages via their appropriate dispatching methods
void SharedStateDistributor::FlushDistribution()
{

	// there isnt really a reason to do it this way (sort by consumer that is), we can let the message sender handle the caching.
	// will leave it for now, but TODO
	//JSONBuilder builder;
	for (unsigned int i = 0; i < consumers.size(); ++i) 
	{
		auto consumer = consumers[i];
		MessageBuilder& builder = MessageBuilderFactory::GetBuilder(consumers[i]->id);
		for (unsigned int j = 0; j < consumer->sharedStateCache.size(); ++j)
		{
			auto sharedStateCache = consumer->sharedStateCache[j];
			if (sharedStateCache->toBeDistributed)
			{
				if (sharedStateCache->attributes.delta)
					builder.Add(sharedStateCache->attributes.id,boost::apply_visitor(SharedState_tostring(), sharedStateCache->delta),sharedStateCache->numChanges);
				else
					builder.Add(sharedStateCache->attributes.id,boost::apply_visitor(SharedState_tostring(), sharedStateCache->value),sharedStateCache->numChanges);

				
				
			}
		}
		std::string finalMessage = builder.Get(currentFrame);
		if (finalMessage.compare("") != 0)
		{
			MessageDispatchController& dispatchController = MessageDispatchController::GetInstance();
			dispatchController.Send(finalMessage, consumers[i]->id);
		}

	}

}

void SharedStateDistributor::LevelReset()
{
	for (unsigned int i =0; i < distributions.size(); ++i)
	{
		if (distributions[i]->attributes.resetOnRestart)
		{
			distributions[i]->numChanges = 0;
		}
	}
}
