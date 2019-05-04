#pragma once

#include <array>
#include <memory>
#include <optional>
#include <vector>
#include "Helpers/NonCopyable.hpp"
#include "Helpers/Reference.hpp"
#include "Scenes/Component.hpp"
#include "ComponentFilter.hpp"
#include "Scenes/Entity.hpp"

namespace ecs
{
class ComponentHolder :
	public NonCopyable
{
public:
	ComponentHolder() = default;

	~ComponentHolder() = default;

	/**
	 * Checks whether the Entity has the Component or not.
	 * @tparam T The Component type.
	 * @param id The Entity ID.
	 * @return If the Entity has the Component.
	 */
	template<typename T>
	bool HasComponent(const Entity::Id &id) const
	{
		// Is the Entity ID and the Component type ID known.
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

	/**
	 * Gets the Component from the Entity.
	 * @tparam T The Component type.
	 * @param id The Entity ID.
	 * @return The Component.
	 */
	template<typename T>
	T &GetComponent(const Entity::Id &id)
	{
		auto component = GetComponentPtr<T>(id);

		if (!component.has_value() || component.value().get() == nullptr)
		{
			throw std::runtime_error("Entity does not have requested Component");
		}

		return *static_cast<T *>(component.value()->get());
	}

	/**
	 * Adds the Component to the Entity.
	 * @tparam T The Component type.
	 * @param id The Entity ID.
	 * @param component The component.
	 */
	template<typename T>
	void AddComponent(const Entity::Id &id, std::unique_ptr<T> &&component)
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

	/**
	 * Removes the Component from the Entity.
	 * @tparam T The Component type.
	 * @param id The Entity ID.
	 */
	template<typename T>
	void RemoveComponent(const Entity::Id &id)
	{
		auto component = GetComponentPtr<T>(id);

		if (component.has_value())
		{
			component.value()->reset();
			m_componentsMasks[id].reset(GetComponentTypeId<T>());
		}
	}

	/**
	 * Removes all Components from the Entity.
	 * @param id The Entity ID.
	 */
	void RemoveAllComponents(const Entity::Id &id);

	/**
	 * Gets the Component mask for the given Entity.
	 * @param id The Entity ID.
	 * @return The Component mask.
	 */
	ComponentFilter::Mask GetComponentsMask(const Entity::Id &id) const;

	/**
	 * Resizes the Component array.
	 * @param size The new size.
	 */
	void Resize(const std::size_t &size);

	/**
	 * Clear all Components.
	 */
	void Clear() noexcept;

private:
	template<typename T>
	std::optional<Reference<std::unique_ptr<Component>>> GetComponentPtr(const Entity::Id &id)
	{
		if (!HasComponent<T>(id))
		{
			return {};
		}

		return m_components[id][GetComponentTypeId<T>()];
	}

	// The index of this array matches the Component type ID.
	using ComponentArray = std::array<std::unique_ptr<Component>, MAX_COMPONENTS>;

	// List of all Components of all Entities.
	// The index of this array matches the Entity ID.
	std::vector<ComponentArray> m_components;

	// List of all masks of all Composents of all Entities.
	// The index of this array matches the Entity ID.
	std::vector<ComponentFilter::Mask> m_componentsMasks;
};
}
