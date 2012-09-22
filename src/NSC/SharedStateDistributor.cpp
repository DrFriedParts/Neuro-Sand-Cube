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
void SharedStateDistributor::AddSharedState(boost::variant<int&,float&, bool&> value, std::string id)
{
	boost::shared_ptr<SharedState> sharedState = boost::shared_ptr<SharedState>(new SharedState(value, id));
	sharedStates.push_back(sharedState);
}

boost::shared_ptr<SharedState> SharedStateDistributor::FindSharedState(std::string stateID)
{
	for (unsigned int i =0; i < sharedStates.size(); ++i)
	{
		if (sharedStates[i]->id.compare(stateID) == 0)
		{
			return sharedStates[i];
		}
	}
	return boost::shared_ptr<SharedState>();

}


void SharedStateDistributor::AddDistribution(SharedStateAttributes attributes)
{

	boost::shared_ptr<SharedStateDistribution> distribution = boost::shared_ptr<SharedStateDistribution>(new SharedStateDistribution());
	/*distribution->onlyOnChange = attributes.onlyOnChange;
	distribution->resetOnRestart = attributes.resetOnRestart;
	distribution->interval = attributes.interval;
	distribution->consumers = attributes.consumers;*/
	distribution->attributes = attributes;

	distribution->framesPassedSinceDistribution = 0;
	distribution->numChanges = 0;
	distribution->toBeDistributed = false;

	// add shared state

	distribution->data = FindSharedState(attributes.id);

	assert(distribution->data.get() != NULL);

	distributions.push_back(distribution);

	// add consumers
	//for (unsigned int i = 0; i < attributes.consumers.size(); ++i)
	{
		//AddConsumerDistribution(attributes.consumers[i], distribution);
		AddConsumerDistribution(distribution);
	}

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
		bool hasChanged = !(boost::apply_visitor(SharedState_equals(), distribution->data->value , distribution->data->prevValue));
		if ((distribution->attributes.onlyOnChange && hasChanged) || (distribution->framesPassedSinceDistribution >= distribution->attributes.interval && !distribution->attributes.onlyOnChange))
		{
			// distribute
			//AddDistributionMessage(distribution);
			distribution->framesPassedSinceDistribution = 0;
			distribution->toBeDistributed = true;

		}
		else
		{
			distribution->toBeDistributed = false;
		}

		if (hasChanged)
			++distribution->numChanges;

		++distribution->framesPassedSinceDistribution;

		distribution->data->prevValue = distribution->data->value;
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
				builder.AddMessage(*consumers[i]->sharedStateCache[j]);
		}
		std::string finalMessage = builder.GetMessage(currentFrame);
		if (finalMessage.compare("") != 0)
		{
			MessageDispatchController& dispatchController = MessageDispatchController::GetInstance();
			dispatchController.Send(finalMessage, consumers[i]->id);
		}

	}

}