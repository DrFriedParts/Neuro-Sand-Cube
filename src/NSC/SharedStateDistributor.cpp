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

//template <class T>
/*void SharedStateDistributor::AddSharedState(SharedState2 value, std::string id)
{
	boost::shared_ptr<SharedState> sharedState = boost::shared_ptr<SharedState>(new SharedState(value, id));
	sharedStates.push_back(sharedState);
}*/

void SharedStateDistributor::AddSharedState(std::string id, SharedStateFunctor functor)
{
	sharedStates[id] = functor;
	//boost::shared_ptr<SharedState> sharedState = boost::shared_ptr<SharedState>(new SharedState(value, id));
	//sharedStates.push_back(sharedState);
}

/*boost::shared_ptr<SharedState> SharedStateDistributor::FindSharedState(std::string stateID)
{
	for (unsigned int i =0; i < sharedStates.size(); ++i)
	{
		if (sharedStates[i]->id.compare(stateID) == 0)
		{
			return sharedStates[i];
		}
	}
	return boost::shared_ptr<SharedState>();

}*/
/*
SharedStateFunctor* SharedStateDistributor::FindSharedStateFunctor(std::string stateID)
{
	auto iterator = sharedStates.find(stateID);

	if (iterator != m_DispatchPool.end())
		return iterator->second;
	else
		return boost::shared_ptr<MessageDispatcher>();
}
*/

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

	// add consumers
	//for (unsigned int i = 0; i < attributes.consumers.size(); ++i)
	{
		//AddConsumerDistribution(attributes.consumers[i], distribution);
		AddConsumerDistribution(distribution);
	}

	return true;

}

void SharedStateDistributor::AddConsumerDistribution(std::string id, boost::shared_ptr<SharedStateDistribution> distribution)
{


}

void SharedStateDistributor::AddDistributionMessage(boost::shared_ptr<SharedStateDistribution> distribution)
{
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

void SharedStateDistributor::AddConsumerDistribution(boost::shared_ptr<SharedStateDistribution> distribution)
{
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
	// distribute

	for (unsigned int i =0; i < distributions.size(); ++i)
	{
		boost::shared_ptr<SharedStateDistribution> distribution = distributions[i];
		distribution->value = distribution->valueFunction();
		bool hasChanged = !(boost::apply_visitor(SharedState_equals(), distribution->value , distribution->prevValue));
		

		if ((distribution->attributes.onlyOnChange && hasChanged) || (distribution->framesPassedSinceDistribution >= distribution->attributes.interval && !distribution->attributes.onlyOnChange))
		{
			// distribute
			//AddDistributionMessage(distribution);
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

		// tk: this will now only be done when the data is distributed
		//distribution->prevValue = distribution->value;
	}

	FlushDistribution();

	++currentFrame;
}

void SharedStateDistributor::ClearDistributionCache()
{
	//consumers.clear();
}

// this will dispatch the messages via their appropriate dispatching methods
void SharedStateDistributor::FlushDistribution()
{

	// there isnt really a reason to do it this way, we can let the message sender handle the caching.
	// will leave it for now, but TODO
	for (unsigned int i = 0; i < consumers.size(); ++i) 
	{
		MessageBuilder& builder = MessageBuilderFactory::GetBuilder(consumers[i]->id);
		for (unsigned int j = 0; j < consumers[i]->sharedStateCache.size(); ++j)
		{
			if (consumers[i]->sharedStateCache[j]->toBeDistributed)
				builder.Add(*consumers[i]->sharedStateCache[j]);
		}
		std::string finalMessage = builder.Get(currentFrame);
		if (finalMessage.compare("") != 0)
		{
			MessageDispatchController& dispatchController = MessageDispatchController::GetInstance();
			dispatchController.Send(finalMessage, consumers[i]->id);
		}

	}

}