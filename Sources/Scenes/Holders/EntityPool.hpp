#pragma once

#include "Utils/NonCopyable.hpp"
#include "Scenes/Entity.hpp"

namespace acid {
class ACID_EXPORT EntityPool : public NonCopyable {
public:
	EntityPool() = default;
	~EntityPool() = default;

	/**
	 * Creates an Entity ID.
	 * @return The Entity ID.
	 */
	Entity::Id Create();

	/**
	 * Stores an Entity ID.
	 * @param id The Entity ID.
	 */
	void Store(Entity::Id id);

	/**
	 * Removes all Entity IDs stored within the pool and resets the next Entity ID value.
	 */
	void Reset() noexcept;

private:
	/// List of stored Entities IDs that are not in use.
	std::vector<Entity::Id> storedIds;

	/// The next available Entity ID.
	Entity::Id nextId = 0;
};
}
