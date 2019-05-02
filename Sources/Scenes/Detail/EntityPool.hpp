#pragma once

#include <vector>
#include "Helpers/NonCopyable.hpp"
#include "Scenes/Entity.hpp"

namespace ecs
{
class EntityPool :
	public NonCopyable
{
public:
	EntityPool() = default;

	~EntityPool() = default;

	EntityPool(EntityPool &&) = default;

	EntityPool &operator=(EntityPool &&) = default;

	// Create an Entity ID
	Entity::Id Create();

	// Store an Entity ID
	void Store(Entity::Id id);

	// Remove all Entity IDs stored within the pool and
	// reset the next Entity ID value
	void Reset() noexcept;

private:
	// List of stored Entities IDs
	std::vector<Entity::Id> m_storedIds;

	// Next Entity ID
	Entity::Id m_nextId{ 0 };
};
}
