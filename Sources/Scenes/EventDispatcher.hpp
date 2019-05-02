#pragma once

#include <functional>
#include <unordered_map>
#include <type_traits>
#include <utility>
#include "Detail/TypeInfo.hpp"
#include "Event.hpp"

namespace ecs
{
class EventDispatcher
{
public:
	EventDispatcher() = default;

	~EventDispatcher() = default;

	EventDispatcher(EventDispatcher const &) = delete;

	EventDispatcher(EventDispatcher &&) = default;

	EventDispatcher &operator=(EventDispatcher const &) = delete;

	EventDispatcher &operator=(EventDispatcher &&) = default;

	// Emit Event T
	template<class T>
	void Emit(T const &evt = T()) const
	{
		static_assert(std::is_base_of<Event, T>::value, "T must be an Event.");

		const auto range = m_listeners.equal_range(GetEventTypeId<T>());

		for (auto it = range.first; it != range.second; ++it)
		{
			it->second.func(&evt);
		}
	}

	// Connect function Func to Event T
	template<class T, class Func>
	Event::Id Connect(Func &&func)
	{
		static_assert(std::is_base_of<Event, T>::value, "T must be an Event.");

		auto slot = [func](void const *evt)
		{
			func(*static_cast<T const *>(evt));
		};

		// Event handler ID
		const auto id = m_nextId;
		++m_nextId;

		m_listeners.insert({ GetEventTypeId<T>(), EventReceiverAttributes{ id, slot }});

		return id;
	}

	// Clear all connected functions to Event T
	template<class T>
	void Clear()
	{
		static_assert(std::is_base_of<Event, T>::value, "T must be an Event.");

		m_listeners.erase(GetEventTypeId<T>());
	}

	// Clear connected function ID
	void Clear(Event::Id id);

	// Clear all Events
	void ClearAll();

private:
	using EventReceiver = std::function<void(void const *)>;

	struct EventReceiverAttributes
	{
		Event::Id id;
		EventReceiver func;
	};

	// Lister list
	std::unordered_multimap<TypeId, EventReceiverAttributes> m_listeners;

	// Next Event handler ID
	Event::Id m_nextId;
};
}
