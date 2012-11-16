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


void StateDistributor::AddState(std::string id, StateFunctor functor, bool event)
{
	states[id] = functor;
	
}

void StateDistributor::AddEvent(std::string id, EventFunctor e)
{
	events[id] = e;
	
}


bool StateDistributor::AddDistribution(StateAttributes attributes)
{

	boost::shared_ptr<StateDistribution> distribution = boost::shared_ptr<StateDistribution>(new StateDistribution());
	
	distribution->attributes = attributes;

	distribution->framesPassedSinceDistribution = 0;
	distribution->numChanges = 0;
	distribution->toBeDistributed = false;
	distribution->event = false;

	// add shared state

	auto iterator = states.find(attributes.id);
		
	if (iterator != states.end())
		distribution->valueFunction = iterator->second;
	else
	{
		// check events
		auto iterator = events.find(attributes.id);
		if (iterator != events.end())
		{
			distribution->eventFunction = iterator->second;
			distribution->event = true;
		}
		else
			return false;
	}

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
		if (distribution->event)
		{
			NSCEvent e = distribution->eventFunction();
			distribution->value = State(e.triggered);
			distribution->eventDescription = e.description;
		}
		else
		{
			distribution->value = distribution->valueFunction();
		}
		bool hasChanged = !(boost::apply_visitor(State_equals(), distribution->value , distribution->prevValue));
		// this is a hack to get booleans to work differently to other variables - they should only transmit if they have just become true
		// this is because the bools used so far are actually events and not bools at all. so this should be changed somehow soon
		if (hasChanged && distribution->value.which() == 2) // == bool
		{
			if (boost::apply_visitor(State_equals(), distribution->value , State(false)))
			{
				hasChanged = false;
				distribution->prevValue = State(false);
			}
		}
			
		
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
				// TK: HACK : temporary hack to let the neurotrigger firmware still issue rewards, this will be removed as soon as the firmware is updated
				std::string id = state->attributes.id;
				std::string value;
				bool str = false;

				if (state->event)
				{
					value = "\"" +state->eventDescription + "\"";
					str = true;
				}
				else if (state->attributes.delta)
					value = boost::apply_visitor(State_tostring(), state->delta);
				else
					value = boost::apply_visitor(State_tostring(), state->value);

				if (subscriber->id.substr(0,3).compare("COM") == 0)
				{
					if (id.compare("reward_issued") == 0)
						id = "level_restart";
				}
				
				builder.Add(id,value,state->numChanges);
			}
		}


		std::string finalMessage = builder.Get(currentFrame);
		if (finalMessage.compare("") != 0)
		{
			subscriber->Send(finalMessage);
		}

	}
}

void StateDistributor::ResetCounters()
{
	for (unsigned int i =0; i < distributions.size(); ++i)
	{
		if (distributions[i]->attributes.resetOnRestart)
		{
			distributions[i]->numChanges = 0;
		}
	}
}


void StateDistributor::ResetCounter(std::string id)
{
	for (unsigned int i =0; i < distributions.size(); ++i)
	{
		if (distributions[i]->attributes.id.compare(id) == 0)
		{
			distributions[i]->numChanges = 0;
		}
	}
}