#pragma once

#include <type_traits>
#include "Helpers/TypeInfo.hpp"

namespace ecs
{
// The maximum number of Components an Entity can holds.
constexpr std::size_t MAX_COMPONENTS{64};

class Component
{
};

/**
 * Gets the Type ID for the Component.
 * @tparam T The Component type.
 * @return The Type ID.
 */
template<typename T>
TypeId GetComponentTypeId() noexcept
{
	static_assert(std::is_base_of<Component, T>::value, "T must be a Component.");

	return TypeInfo<Component>::GetTypeId<T>();
}
}
