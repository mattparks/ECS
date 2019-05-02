#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "Helpers/NonCopyable.hpp"
#include "Helpers/Reference.hpp"
#include "Detail/ComponentFilter.hpp"
#include "Detail/TypeInfo.hpp"
#include "Entity.hpp"
#include "Event.hpp"

namespace ecs
{
namespace detail
{
class SystemHolder;
}

class System :
	public NonCopyable
{
public:
	System(System &&) = default;

	virtual ~System();

	System &operator=(System &&) = default;

	// Get Entities attached to this System
	const std::vector<Entity> &GetEntities() const;

	// Get the World that the System belongs to
	World &GetWorld();

	// Get the World that the System belongs to
	const World &GetWorld() const;

	// Get Entity count
	std::size_t GetEntityCount() const noexcept;

	// Detach all entities
	void DetachAll();

	// Iterate through all enabled Entities
	template<typename Func>
	void ForEach(Func &&func);

	// Triggered on System start up
	virtual void OnStart();

	// Triggered on System shut down
	virtual void OnShutdown();

	// Triggered before each refresh
	virtual void OnPreUpdate(const float &delta);

	// Triggered for each refresh
	virtual void OnUpdate(const float &delta);

	// Triggered after each refresh
	virtual void OnPostUpdate(const float &delta);

	// Triggered when an Entity has been attached to the System
	virtual void OnEntityAttached(Entity entity);

	// Triggered when an Entity has been detached from the System
	virtual void OnEntityDetached(Entity entity);

	// Triggered when an Entity has been enabled
	virtual void OnEntityEnabled(Entity entity);

	// Triggered when an Entity has been disabled
	virtual void OnEntityDisabled(Entity entity);

protected:
	// This class must be inherited
	System() = default;

	// Access to the filter
	ComponentFilter &GetFilter();

	// Emit Event T
	template<typename T>
	void EmitEvent(const T &evt) const;

	// Connect function Func to Event T
	template<typename T, typename Func>
	Event::Id ConnectEvent(Func &&func);

	// Clear connected function ID
	void DisconnectEvent(const Event::Id &id);

	// Clear all Events
	void DisconnectAllEvents();

private:
	enum class EntityStatus
	{
		NotAttached, Enabled, Disabled
	};

	// Attach an Entity to the System
	void AttachEntity(const Entity &entity);

	// Detach an Entity from the System
	void DetachEntity(const Entity &entity);

	// Enable Entity
	void EnableEntity(const Entity &entity);

	// Disable Entity
	void DisableEntity(const Entity &entity);

	// Call an event
	template<typename Func>
	void CallEvent(Func &&func);

	// Start event
	void StartEvent();

	// Shutdown event
	void ShutdownEvent();

	// Pre-update event
	void PreUpdateEvent(const float &delta);

	// Update event
	void UpdateEvent(const float &delta);

	// Post-update event
	void PostUpdateEvent(const float &delta);

	// Attach event
	void AttachEvent(const Entity &entity);

	// Detach event
	void DetachEvent(const Entity &entity);

	// Enable event
	void EnableEvent(const Entity &entity);

	// Disable event
	void DisableEvent(const Entity &entity);

	// Get Entity status
	EntityStatus GetEntityStatus(const Entity::Id &id) const;

	// Set Entity status
	void SetEntityStatus(const Entity::Id &id, const EntityStatus &status);

	// Enabled Entities attached to this System
	std::vector<Entity> m_enabledEntities;

	// Disabled Entities attached to this System
	std::vector<Entity> m_disabledEntities;

	// Entities status (enabled/disabled)
	std::unordered_map<Entity::Id, EntityStatus> m_status;

	// The World that this System belongs to
	std::optional<Reference<World>> m_world;

	// The mask that the Entities must matched to be
	// attached to this System
	ComponentFilter m_filter;

	// List of the Events this System is listening to
	std::unordered_set<std::size_t> m_events;

	// Only World can access ComponentFilter
	friend class World;

	// SystemHolder needs to trigger onShutdown event
	friend class SystemHolder;
};

// Get the Type ID for the System T
template<typename T>
TypeId GetSystemTypeId() noexcept;
}
