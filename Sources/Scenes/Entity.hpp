#pragma once

#include <optional>
#include "Helpers/Reference.hpp"
#include "Component.hpp"

namespace ecs
{
class World;

class Entity
{
public:
	// Entity ID type.
	using Id = std::size_t;

	Entity() = default;

	Entity(const Id &id, World &world);

	~Entity() = default;

	/**
	 * Casts the Entity into its ID.
	 * @return The Entity ID.
	 */
	operator Id() const noexcept;

	/**
	 * Gets the Entity ID.
	 * @return The Entity ID.
	 */
	Id GetId() const noexcept;

	/**
	 * Adds the Component to the Entity.
	 * @tparam T The Component type.
	 * @tparam Args The arg type.
	 * @param args The arguments.
	 * @return The Component.
	 */
	template<typename T, typename... Args>
	T &AddComponent(Args &&...args);

	/**
	 * Gets the Component from the Entity.
	 * @tparam T The Component type.
	 * @return The Component.
	 */
	template<typename T>
	T &GetComponent();

	/**
	 * Gets the Component from the Entity.
	 * @tparam T The Component type.
	 * @return The Component.
	 */
	template<typename T>
	const T &GetComponent() const;

	/**
	 * Checks whether the Entity has the Component or not.
	 * @tparam T The Component type.
	 * @return If the Entity has the Component.
	 */
	template<typename T>
	bool HasComponent() const;

	/**
	 * Removes the Component from the Entity.
	 * @tparam T The Component type.
	 */
	template<typename T>
	void RemoveComponent();

	/**
	 * Removes all components from the Entity.
	 */
	void RemoveAllComponents();

	/**
	 * Enables the Entity.
	 */
	void Enable();

	/**Disables the Entity.
	 *
	 */
	void Disable();

	/**
	 * Gets whether the Entity is enabled or not.
	 * @return If the Entity is enabled.
	 */
	bool IsEnabled() const;

	/**
	 * Gets whether the Entity is valid or not.
	 * @return If the Entity is valid.
	 */
	bool IsValid() const;

	/**
	 * Gets the Entity name.
	 * @return The Entity name.
	 */
	std::string GetName() const;

	/**
	 * Removes the Entity.
	 */
	void Remove();

	bool operator==(const Entity &other) const;

	bool operator!=(const Entity &other) const;

private:
	// Entity ID.
	Id m_id = 0;

	// The World that this Entity belongs to.
	std::optional<Reference<World>> m_world;
};
}

namespace std
{
template<>
struct hash<ecs::Entity>
{
	size_t operator()(const ecs::Entity &entity) const noexcept
	{
		return hash<ecs::Entity::Id>()(entity.GetId());
	}
};
}
