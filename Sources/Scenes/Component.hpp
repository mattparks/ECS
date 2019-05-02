#pragma once

#include <type_traits>
#include "Detail/TypeInfo.hpp"

namespace ecs
{
// The maximum number of Components an Entity can holds
constexpr std::size_t MAX_COMPONENTS = 64;

class Component
{
public:
	Component() = default;

	virtual ~Component() = default;
};

// Get the Type ID for the Component T
template<class T>
TypeId GetComponentTypeId() noexcept
{
	static_assert(std::is_base_of<Component, T>::value, "T must be a Component.");

	return TypeInfo<Component>::GetTypeId<T>();
}
}
