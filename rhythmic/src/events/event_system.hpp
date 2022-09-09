#ifndef EVENT_SYSTEM_HPP_
#define EVENT_SYSTEM_HPP_

#include <functional>

#include "events_common.hpp"

#define CREATE_MEMBER_CB(cb) std::bind(&cb, this, std::placeholders::_1)

namespace Rhythmic
{
	typedef std::function<void(void*)> EventFunc;

	struct EventListener
	{
		int eventType;
		int key;
		bool wasRemoved;
	};


	/*
	
	The event system is very primitive

	You can dispatch events, create and remove listeners

	One problem is that you can't create/remove listeners during an event

	This is where the queue comes into play, where callbacks can queue the removal or creation of a listener

	Since the queue may or may not mess up event dispatching, there is a queue for event dispatching too

	The queue is ran through after an event, or after a tick
	
	*/


	/*
	Dispatches a message with a void* for data
	*/
	void EventDispatch(int event_type, void *data = 0);

	/*
	Adds an event listener

	Event Types that are negative can only have one listener (useful for event listeners that return a value)

	EventListener returned is used to remove the listener later
	*/
	EventListener EventAddListener(int event_type, EventFunc function);

	/*
	Queues an event listener to be added

	This function is necessary because listeners can not be added nor removed when an event is currently executing

	Good for creating listeners when an event is triggered
	*/
	void EventQueueAddListener(EventListener *listener, int event_type, EventFunc function);

	/*
	Queues an event listener to be removed

	Good for removing listeners when an event is triggered
	*/
	void EventQueueRemoveListener(EventListener *listener);

	/*
	Queues another event to dispatch
	*/
	void EventQueueDispatch(int event_type, void *data = 0);

	/*
	Updates the queue to keep the queue empty
	*/
	void EventUpdateQueue();

	/*
	Removes the listener
	*/
	void EventRemoveListener(EventListener &listener);

	/*
	Adds an event listener to the event system on creation
	Removes the event listener when deleted or when it leaves scope
	*/
	class EventListener_raii
	{
	public:
		EventListener_raii(int event_type, EventFunc func)
		{
			m_listener = EventAddListener(event_type, func);
		}
		~EventListener_raii()
		{
			EventRemoveListener(m_listener);
		}
	private:
		EventListener m_listener;
	};
}

#endif

