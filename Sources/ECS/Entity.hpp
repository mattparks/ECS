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
	// Entity ID type
	using Id = std::size_t;

	class Hash
	{
	public:
		// Compute Entity hash
		std::size_t operator()(Entity const &entity) const;
	};

	Entity() = default;

	~Entity() = default;

	Entity(Id id, World &world);

	Entity(Entity const &) = default;

	Entity(Entity &&) noexcept = default;

	Entity &operator=(Entity const &) = default;

	Entity &operator=(Entity &&) noexcept = default;

	// Cast Entity into its ID
	operator Id() const noexcept;

	// Get the Entity ID
	Id GetId() const noexcept;

	// Add the Component T to the Entity
	template<class T, class... Args>
	T &AddComponent(Args &&...args);

	// Get the Component T from the Entity
	template<class T>
	T &GetComponent();

	// Get the Component T from the Entity
	template<class T>
	T const &GetComponent() const;

	// Check whether the Entity has the Component T or not
	template<class T>
	bool HasComponent() const;

	// Remove the Component T from the Entity
	template<class T>
	void RemoveComponent();

	// Remove all components from the Entity
	void RemoveAllComponents();

	// Enable the Entity
	void Enable();

	// Disable the Entity
	void Disable();

	// Check whether the Entity is enabled or not
	bool IsEnabled() const;

	// Check whether the Entity is valid or not
	bool IsValid() const;

	// Get Entity name
	std::string GetName() const;

	// Remove the Entity
	void Remove();

	bool operator==(Entity const &rhs) const;

	bool operator!=(Entity const &rhs) const;

private:
	// Entity ID
	Id m_id{ 0 };

	// The World that this Entity belongs to
	std::optional<Reference<World>> m_world;
};
}
