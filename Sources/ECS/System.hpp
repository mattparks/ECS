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
#include "EventDispatcher.hpp"

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
	std::vector<Entity> const &GetEntities() const;

	// Get the World that the System belongs to
	World &GetWorld();

	// Get the World that the System belongs to
	World const &GetWorld() const;

	// Get Entity count
	std::size_t GetEntityCount() const noexcept;

	// Detach all entities
	void DetachAll();

	// Iterate through all enabled Entities
	template<class Func>
	void ForEach(Func &&func);

	// Triggered on System start up
	virtual void OnStart();

	// Triggered on System shut down
	virtual void OnShutdown();

	// Triggered before each refresh
	virtual void OnPreUpdate(float elapsed);

	// Triggered for each refresh
	virtual void OnUpdate(float elapsed);

	// Triggered after each refresh
	virtual void OnPostUpdate(float elapsed);

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
	detail::ComponentFilter &GetFilter();

	// Emit Event T
	template<class T>
	void EmitEvent(T const &evt) const;

	// Connect function Func to Event T
	template<class T, class Func>
	Event::Id ConnectEvent(Func &&func);

	// Clear connected function ID
	void DisconnectEvent(Event::Id id);

	// Clear all Events
	void DisconnectAllEvents();

private:
	enum class EntityStatus
	{
		NotAttached, Enabled, Disabled
	};

	// Attach an Entity to the System
	void AttachEntity(Entity const &entity);

	// Detach an Entity from the System
	void DetachEntity(Entity const &entity);

	// Enable Entity
	void EnableEntity(Entity const &entity);

	// Disable Entity
	void DisableEntity(Entity const &entity);

	// Call an event
	template<class Func>
	void CallEvent(Func &&func);

	// Start event
	void StartEvent();

	// Shutdown event
	void ShutdownEvent();

	// Pre-update event
	void PreUpdateEvent(float elapsed);

	// Update event
	void UpdateEvent(float elapsed);

	// Post-update event
	void PostUpdateEvent(float elapsed);

	// Attach event
	void AttachEvent(Entity const &entity);

	// Detach event
	void DetachEvent(Entity const &entity);

	// Enable event
	void EnableEvent(Entity const &entity);

	// Disable event
	void DisableEvent(Entity const &entity);

	// Get Entity status
	EntityStatus GetEntityStatus(Entity::Id id) const;

	// Set Entity status
	void SetEntityStatus(Entity::Id id, EntityStatus status);

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
	detail::ComponentFilter m_filter;

	// List of the Events this System is listening to
	std::unordered_set<std::size_t> m_events;

	// Only World can access detail::ComponentFilter
	friend class World;

	// detail::SystemHolder needs to trigger onShutdown event
	friend class detail::SystemHolder;
};

// Get the Type ID for the System T
template<class T>
detail::TypeId GetSystemTypeId() noexcept;
}
