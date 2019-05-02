#pragma once

#include <array>
#include <memory>
#include <optional>
#include <vector>
#include "Helpers/NonCopyable.hpp"
#include "Helpers/Reference.hpp"
#include "Scenes/Component.hpp"
#include "Scenes/Entity.hpp"
#include "ComponentFilter.hpp"
#include "TypeInfo.hpp"

namespace ecs
{
class ComponentHolder :
	public NonCopyable
{
public:
	ComponentHolder() = default;

	~ComponentHolder() = default;

	ComponentHolder(ComponentHolder &&) = default;

	ComponentHolder &operator=(ComponentHolder &&) = default;

	// Add the component T to the Entity
	template<typename T>
	void AddComponent(Entity::Id id, std::unique_ptr<T> &&component)
	{
		if (id >= m_components.size())
		{
			throw std::runtime_error("Entity ID is out of range");
		}

		const auto typeId = GetComponentTypeId<T>();

		if (typeId >= m_components[id].size())
		{
			throw std::runtime_error("Component type ID is out of range");
		}

		m_components[id][typeId] = std::move(component);
		m_componentsMasks[id].set(typeId);
	}

	// Get the Component T from the Entity
	template<typename T>
	T &GetComponent(Entity::Id id)
	{
		auto component = GetComponentPtr<T>(id);

		if (!component.has_value() || component.value().get() == nullptr)
		{
			throw std::runtime_error("Entity does not have requected Component");
		}

		return *static_cast<T *>(component.value()->get());
	}

	// Check whether the Entity has the Component T or not
	template<typename T>
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
	template<typename T>
	void RemoveComponent(Entity::Id id)
	{
		auto component = GetComponentPtr<T>(id);

		if (component.has_value())
		{
			component.value()->reset();
			m_componentsMasks[id].reset(GetComponentTypeId<T>());
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
	template<typename T> std::optional<Reference < std::unique_ptr<Component>>>
	GetComponentPtr(Entity::Id id)
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
