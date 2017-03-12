/**
 * @file EventManager.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * The EventManager system for the Chimera framework.
 */
#pragma once

#include <map>

#include "Config.h"

#include "Callback.h"
#include "CircularQueue.h"
#include "Events/Event.h"

namespace Sim {
	typedef util::Callback <void (unsigned int)> EventListener;

	class EventManager {

		private:
			unsigned int _index;
			CircularQueue <Event, SIM_MAX_EVENT_QUEUE_SIZE> _queue;
			std::map <unsigned int, EventListener> _listeners;

		private: // forbidden copy constructor and assignment operator
			EventManager (const EventManager&);
			EventManager& operator = (const EventManager&);

		public:
			EventManager ();
			~EventManager ();

			bool AddListener (const EventListener&, EventType, unsigned int);
			bool RemoveListener (const EventListener&, EventType, unsigned int);

			bool QueueEvent (Event&);

			bool Initialize (const char* config);
			void Cleanup ();
	};
}
