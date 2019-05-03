#pragma once

#include <type_traits>
#include "Helpers/TypeInfo.hpp"

namespace ecs
{
class Event
{
public:
	// Event ID type.
	using Id = std::size_t;
};

/**
 * Gets the Type ID for the Event.
 * @tparam T The Event type.
 * @return The Type ID.
 */
template<typename T>
TypeId GetEventTypeId() noexcept
{
	static_assert(std::is_base_of<Event, T>::value, "T must be an Event.");

	return TypeInfo<Event>::GetTypeId<T>();
}
}
