/**
 * @file EventManager.h
 * @author Kishalay Kundu <kishalay.kundu@gmail.com>
 * @section LICENSE
 * See LICENSE.txt included in this package
 *
 * @section DESCRIPTION
 * See EventManager.h.
 */

#include "Preprocess.h"
#include "Events/EventManager.h"

namespace Sim {

	EventManager::EventManager ()
	: _index (0)
	{
		LOG ("Event manager constructed");
	}

	EventManager::~EventManager ()
	{
		Cleanup ();
		LOG ("Event manager destroyed");
	}

	EventManager::EventManager (const EventManager& em) : _index (em._index) {}

	EventManager& EventManager::operator = (const EventManager& em) {return *this;}

	bool EventManager::AddListener (const EventListener& l, EventType e, unsigned int a)
	{
		unsigned int key = a;
		key |= e << sizeof (unsigned short);

		_listeners.emplace (std::make_pair(key, l));
		return true;
	}

	bool EventManager::RemoveListener (const EventListener&, EventType e, unsigned int a)
	{
		unsigned int key = a;
		key |= e << sizeof (unsigned short);

		_listeners.erase (key);
		return true;
	}

	bool EventManager::QueueEvent (Event& e)
	{
		_queue [_index].Push(e);
		return true;
	}

	bool EventManager::Initialize (const char* config)
	{
		return true;
	}

	void EventManager::Cleanup ()
	{

	}
}
