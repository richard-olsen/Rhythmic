#include "event_system.hpp"

#include <map>

#include <queue>

#include <wge/core/logging.hpp>

namespace Rhythmic
{
	enum QueueType
	{
		CREATE,
		REMOVE,
		DISPATCH
	};
	struct QueueData
	{
		EventListener *listener;
		QueueType type;
		int event_type;
		EventFunc function;
	};


	std::map<int, std::map<int, EventFunc>> g_eventListeners;
	std::queue<QueueData> g_eventQueue;

	void EventDispatch(int event_type, void *data)
	{
		auto it = g_eventListeners.find(event_type);
		if (it != g_eventListeners.end()) // Event type was found
		{
			for (auto listener = it->second.begin(); listener != it->second.end(); listener++) // Iterate through the available listeners
			{
				if (listener->second)
				{
					listener->second(data); // Call the listener function
				}
			}
		}

		EventUpdateQueue();
	}
	EventListener EventAddListener(int event_type, EventFunc function)
	{
		EventListener listener;
		listener.eventType = event_type;
		listener.wasRemoved = false;
		
		auto it = g_eventListeners.find(event_type); // Find the event type
		
		if (it != g_eventListeners.end() && event_type >= 0) // Event type was found
		{
			int i = 0;

			auto eventListener = it->second.begin();
			while ((eventListener = it->second.find(i)) != it->second.end())
				i++;

			it->second.insert(std::pair<int, EventFunc>(i, function));

			listener.key = i;
		}
		else // Event type wasn't found
		{
			g_eventListeners.insert(std::pair<int, std::map<int, EventFunc>>(event_type, std::map<int, EventFunc>()));
			g_eventListeners.at(event_type).insert(std::pair<int, EventFunc>(0, function));
			listener.key = 0;
		}

		return listener;
	}
	void EventRemoveListener(EventListener &listener)
	{
		if (listener.wasRemoved)
			return;

		auto it = g_eventListeners.find(listener.eventType);

		if (it != g_eventListeners.end())
		{
			auto eventListener = it->second.find(listener.key);
			if (eventListener != it->second.end())
				it->second.erase(eventListener);

			if (it->second.empty())
				g_eventListeners.erase(it);

			listener.wasRemoved = true;
		}
	}

	void EventQueueAddListener(EventListener *listener, int event_type, EventFunc function)
	{
		QueueData data;
		data.event_type = event_type;
		data.function = function;
		data.listener = listener;
		data.type = CREATE;
		g_eventQueue.push(data);
	}


	void EventQueueRemoveListener(EventListener *listener)
	{
		QueueData data;
		data.listener = listener;
		data.type = REMOVE;
		g_eventQueue.push(data);
	}


	void EventQueueDispatch(int event_type, void *data)
	{
		// Implement later
		// Try to figure out a way to send pointers without allocating on the heap
	}


	void EventUpdateQueue()
	{
		while (!g_eventQueue.empty())
		{
			const QueueData &data = g_eventQueue.front();

			switch (data.type)
			{
			case CREATE:
				*data.listener = EventAddListener(data.event_type, data.function);
				break;
			case REMOVE:
				EventRemoveListener(*data.listener);
				break;
			case DISPATCH:
				LOG_INFO("Queueing dispatch events are not implemented yet!");
				break;
			}

			g_eventQueue.pop();
		}
	}
}
