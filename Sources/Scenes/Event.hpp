#pragma once

#include <type_traits>
#include "Detail/TypeInfo.hpp"

namespace ecs
{
struct Event
{
	using Id = std::size_t;
};

// Get the Type ID for the Event T
template<class T>
TypeId GetEventTypeId() noexcept
{
	static_assert(std::is_base_of<Event, T>::value, "T must be an Event.");

	return TypeInfo<Event>::GetTypeId<T>();
}
}
