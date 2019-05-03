#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "Helpers/NonCopyable.hpp"
#include "Helpers/Reference.hpp"
#include "Helpers/Delegate.hpp"
#include "Helpers/TypeInfo.hpp"
#include "ComponentFilter.hpp"
#include "Entity.hpp"

namespace ecs
{
class System :
	public NonCopyable,
	public Observer
{
public:
	System() = default;

	virtual ~System() = default;

	/**
	 * Iterates through all enabled Entities.
	 * @tparam Func The function type.
	 * @param func The function.
	 */
	template<typename Func>
	void ForEach(Func &&func);

	/**
	 * Detaches all entities.
	 */
	void DetachAll();

	/**
	 * Gets Entities attached to this System.
	 * @return The Entities.
	 */
	const std::vector<Entity> &GetEntities() const { return m_enabledEntities; }

	/**
	 * Gets the World that the System belongs to.
	 * @return The World.
	 */
	World &GetWorld();

	/**
	 * Gets the World that the System belongs to.
	 * @return The World.
	 */
	const World &GetWorld() const;

protected:
	/**
	 * Gets the component filter.
	 * @return The component filter.
	 */
	ComponentFilter &GetFilter() { return m_filter; }

	Delegate<void(Entity)> &OnEntityAttach() { return m_onEntityAttach; }

	Delegate<void(Entity)> &OnEntityDetach() { return m_onEntityDetach; }

	Delegate<void(Entity)> &OnEntityEnable() { return m_onEntityEnable; }

	Delegate<void(Entity)> &OnEntityDisable() { return m_onEntityDisable; }

	virtual void Update(const float &delta) = 0;

private:
	friend class World;
	friend class SystemHolder;

	enum class EntityStatus
	{
		NotAttached, Enabled, Disabled
	};

	/**
	 * Attach an Entity to the System.
	 * @param entity
	 */
	void AttachEntity(const Entity &entity);

	/**
	 * Detach an Entity from the System.
	 * @param entity The Entity.
	 */
	void DetachEntity(const Entity &entity);

	/**
	 * Enable Entity.
	 * @param entity The Entity.
	 */
	void EnableEntity(const Entity &entity);

	/**
	 * Disable Entity.
	 * @param entity The Entity.
	 */
	void DisableEntity(const Entity &entity);

	/**
	 * Get Entity status.
	 * @param id The Entity ID.
	 * @return The status.
	 */
	EntityStatus GetEntityStatus(const Entity::Id &id) const;

	/**
	 * Set Entity status.
	 * @param id The Entity ID.
	 * @param status The status.
	 */
	void SetEntityStatus(const Entity::Id &id, const EntityStatus &status);

	// Enabled Entities attached to this System.
	std::vector<Entity> m_enabledEntities;

	// Disabled Entities attached to this System.
	std::vector<Entity> m_disabledEntities;

	// Entities status (enabled/disabled).
	std::unordered_map<Entity::Id, EntityStatus> m_status;

	// The World that this System belongs to.
	std::optional<Reference<World>> m_world;

	// The mask that the Entities must matched to be attached to this System.
	ComponentFilter m_filter;

	Delegate<void(Entity)> m_onEntityAttach;
	Delegate<void(Entity)> m_onEntityDetach;
	Delegate<void(Entity)> m_onEntityEnable;
	Delegate<void(Entity)> m_onEntityDisable;

};

// Get the Type ID for the System T
template<typename T>
TypeId GetSystemTypeId() noexcept;
}
