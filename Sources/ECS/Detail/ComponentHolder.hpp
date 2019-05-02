#pragma once

#include <array>
#include <memory>
#include <optional>
#include <vector>
#include "Helpers/Reference.hpp"
#include "ECS/Exceptions/InvalidComponent.hpp"
#include "ECS/Exceptions/InvalidEntity.hpp"
#include "ECS/Component.hpp"
#include "ECS/Entity.hpp"
#include "ECS/Exceptions/Exception.hpp"
#include "ComponentFilter.hpp"
#include "TypeInfo.hpp"

namespace ecs::detail
{
class ComponentHolder
{
public:
	ComponentHolder() = default;

	~ComponentHolder() = default;

	ComponentHolder(ComponentHolder const &) = delete;

	ComponentHolder(ComponentHolder &&) = default;

	ComponentHolder &operator=(ComponentHolder const &) = delete;

	ComponentHolder &operator=(ComponentHolder &&) = default;

	// Add the component T to the Entity
	template<class T>
	void AddComponent(Entity::Id id, std::unique_ptr<T> &&component)
	{
		if (id >= m_components.size())
		{
			// The Entity ID is out of range
			throw InvalidEntity("ecs::Entity::AddComponent()");
		}

		const auto typeId = GetComponentTypeId<T>();

		if (typeId >= m_components[id].size())
		{
			// The Component type ID is out of range
			throw InvalidComponent("ecs::Entity::AddComponent()");
		}

		m_components[id][typeId] = std::move(component);
		m_componentsMasks[id].set(typeId);
	}

	// Get the Component T from the Entity
	template<class T>
	T &GetComponent(Entity::Id id)
	{
		auto component = GetComponentPtr<T>(id);

		if (!component.has_value() || component.value().get() == nullptr)
		{
			throw Exception("Entity does not have this Component.", "ecs::Entity::GetComponent()");
		}

		return *static_cast<T*>(component.value()->get());
	}

	// Check whether the Entity has the Component T or not
	template<class T>
	bool HasComponent(Entity::Id id) const
	{
		// Is the Entity ID and the Component type ID known
		if (id < m_components.size())
		{
			auto typeId = GetComponentTypeId<T>();

			// Is the Component type ID known
			if (typeId < m_components[id].size())
			{
				return m_components[id][typeId] != nullptr;
			}
		}

		return false;
	}

	// Remove the Component T from the Entity
	template<class T>
	void RemoveComponent(Entity::Id id)
	{
		auto component = GetComponentPtr<T>(id);

		if (component.has_value())
		{
			component.value()->reset();
			m_componentsMasks[id].reset(getComponentTypeId<T>());
		}
	}

	// Remove all components from the Entity
	void RemoveAllComponents(Entity::Id id);

	// Get the Component mask for the given Entity
	ComponentFilter::Mask GetComponentsMask(Entity::Id id) const;

	// Resize the Component array
	void Resize(std::size_t size);

	// Clear all Components
	void Clear() noexcept;

private:
	template<class T> 
	std::optional<Reference<std::unique_ptr<Component>>> GetComponentPtr(Entity::Id id)
	{
		if (!HasComponent<T>(id))
		{
			return std::nullopt;
		}

		return m_components[id][GetComponentTypeId<T>()];
	}

	// The index of this array matches the Component type ID
	using ComponentArray = std::array<std::unique_ptr<Component>, MAX_COMPONENTS>;

	// List of all Components of all Entities
	// The index of this array matches the Entity ID
	std::vector<ComponentArray> m_components;

	// List of all masks of all Composents of all Entities
	// The index of this array matches the Entity ID
	std::vector<ComponentFilter::Mask> m_componentsMasks;
};
}
