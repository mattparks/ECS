#pragma once

#include <optional>
#include "Helpers/Reference.hpp"
#include "Component.hpp"

namespace ecs
{
class Scene;

class Entity
{
public:
	// Entity ID type.
	using Id = std::size_t;

	Entity() = default;

	Entity(const Id &id, Scene &scene);

	~Entity() = default;

	/**
	 * Casts the Entity into its ID.
	 * @return The Entity ID.
	 */
	operator Id() const noexcept { return m_id; }

	/**
	 * Gets the Entity ID.
	 * @return The Entity ID.
	 */
	Id GetId() const noexcept { return m_id; }

	/**
	 * Checks whether the Entity has the Component or not.
	 * @tparam T The Component type.
	 * @return If the Entity has the Component.
	 */
	template<typename T>
	bool HasComponent() const;

	/**
	 * Gets the Component from the Entity.
	 * @tparam T The Component type.
	 * @return The Component.
	 */
	template<typename T>
	T *GetComponent() const;

	/**
	 * Adds the Component to the Entity.
	 * @tparam T The Component type.
	 * @tparam Args The constructor arg types.
	 * @param args The constructor arguments.
	 * @return The Component.
	 */
	template<typename T, typename... Args>
	T *AddComponent(Args &&...args);

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
	 * Gets the Entity name.
	 * @return The Entity name.
	 */
	std::string GetName() const;

	/**
	 * Gets whether the Entity is enabled or not.
	 * @return If the Entity is enabled.
	 */
	bool IsEnabled() const;

	/**
	 * Enables the Entity.
	 */
	void Enable();

	/**Disables the Entity.
	 *
	 */
	void Disable();

	/**
	 * Gets whether the Entity is valid or not.
	 * @return If the Entity is valid.
	 */
	bool IsValid() const;

	/**
	 * Removes the Entity.
	 */
	void Remove();

	bool operator==(const Entity &other) const;

	bool operator!=(const Entity &other) const;

private:
	// Entity ID.
	Id m_id{};

	// The Scene that this Entity belongs to.
	std::optional<Reference<Scene>> m_scene;
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
