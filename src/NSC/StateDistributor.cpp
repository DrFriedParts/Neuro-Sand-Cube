#include "StateDistributor.h"

#include <assert.h>

#include "MessageBuilderFactory.h"

StateDistributor& StateDistributor::GetInstance()
{
	static StateDistributor instance;
	return instance;
}

StateDistributor::StateDistributor(void) : currentFrame(0)
{
}


StateDistributor::~StateDistributor(void)
{
	states.empty();  
	distributions.empty();  
}


void StateDistributor::AddState(std::string id, StateFunctor functor)
{
	states[id] = functor;
	
}


bool StateDistributor::AddDistribution(StateAttributes attributes)
{

	boost::shared_ptr<StateDistribution> distribution = boost::shared_ptr<StateDistribution>(new StateDistribution());
	
	distribution->attributes = attributes;

	distribution->framesPassedSinceDistribution = 0;
	distribution->numChanges = 0;
	distribution->toBeDistributed = false;

	// add shared state

	auto iterator = states.find(attributes.id);
		
	if (iterator != states.end())
		distribution->valueFunction = iterator->second;
	else
		return false;

	distributions.push_back(distribution);

	AddConsumerDistribution(distribution);
	
	return true;

}

void StateDistributor::AddSubscriber(boost::shared_ptr<ConnectionInterface> subscriber)
{
	subscribers.push_back(subscriber);

}


void StateDistributor::AddConsumerDistribution(boost::shared_ptr<StateDistribution> distribution)
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
				consumer->stateCache.push_back(distribution);
				consumerExists = true;
			}
		}
		if (!consumerExists)
		{
			boost::shared_ptr<ConsumerDistributionCache> consumer = boost::shared_ptr<ConsumerDistributionCache>(new ConsumerDistributionCache());
			consumer->id = distribution->attributes.consumers[j];
			consumer->stateCache.push_back(distribution);

			consumers.push_back(consumer);
			consumerMap[consumer->id] = consumer;
		}
	}

}

void StateDistributor::Distribute()
{
	for (unsigned int i =0; i < distributions.size(); ++i)
	{
		boost::shared_ptr<StateDistribution> distribution = distributions[i];
		distribution->value = distribution->valueFunction();
		bool hasChanged = !(boost::apply_visitor(State_equals(), distribution->value , distribution->prevValue));
		
		if ((distribution->attributes.onlyOnChange && hasChanged) || (distribution->framesPassedSinceDistribution >= distribution->attributes.interval && !distribution->attributes.onlyOnChange))
		{
			// distribute
			distribution->framesPassedSinceDistribution = 0;
			distribution->toBeDistributed = true;

			distribution->delta = boost::apply_visitor(State_difference(), distribution->value , distribution->prevValue);
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
void StateDistributor::FlushDistribution()
{

	for (auto iterator = subscribers.begin(); iterator != subscribers.end(); iterator++)
	{
		auto subscriber = *iterator;
		MessageBuilder& builder = MessageBuilderFactory::GetBuilder(subscriber->id);


		// pick a vector of distributions - either consumer specific or all
		auto* requiredStates = &distributions;
		auto consumerIter = consumerMap.find(subscriber->id);
		if (consumerIter != consumerMap.end())
		{
			requiredStates = &((*consumerIter).second->stateCache);
		
		}
		for (auto iterator = requiredStates->begin(); iterator != requiredStates->end(); iterator++)
		{
			auto state = *iterator;
			if (state->toBeDistributed)
			{
				if (state->attributes.delta)
					builder.Add(state->attributes.id,boost::apply_visitor(State_tostring(), state->delta));
				else
					builder.Add(state->attributes.id,boost::apply_visitor(State_tostring(), state->value));
			}
		}


		std::string finalMessage = builder.Get(currentFrame);
		if (finalMessage.compare("") != 0)
		{
			subscriber->Send(finalMessage);
		}

	}
}

void StateDistributor::LevelReset()
{
	for (unsigned int i =0; i < distributions.size(); ++i)
	{
		if (distributions[i]->attributes.resetOnRestart)
		{
			distributions[i]->numChanges = 0;
		}
	}
}
