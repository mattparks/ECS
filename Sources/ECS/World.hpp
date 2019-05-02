#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "Helpers/NonCopyable.hpp"
#include "Helpers/Reference.hpp"
#include "Detail/ComponentFilter.hpp"
#include "Detail/ComponentHolder.hpp"
#include "Detail/EntityPool.hpp"
#include "Detail/SystemHolder.hpp"
#include "Detail/TypeInfo.hpp"
#include "Component.hpp"
#include "Entity.hpp"
#include "EventDispatcher.hpp"
#include "System.hpp"

namespace ecs
{
class World :
	public NonCopyable
{
public:
	World() = default;

	World(World &&) = default;

	~World();

	World &operator=(World &&) = default;

	// Add a System
	template<class T, class... Args>
	T &AddSystem(std::size_t priority = 0, Args &&...args);

	// Get a System
	template<class T>
	T &GetSystem();

	// Get a System
	template<class T>
	T const &GetSystem() const;

	// Check whether a System exists or not
	template<class T>
	bool HasSystem() const;

	// Remove a System
	template<class T>
	void RemoveSystem();

	// Remove all Systems
	void RemoveAllSystems();

	// Create a new Entity
	Entity CreateEntity();

	// Create a new named Entity
	Entity CreateEntity(std::string const &name);

	// Get Entity by ID
	std::optional<ecs::Entity> GetEntity(Entity::Id id) const;

	// Get Entity by name
	std::optional<ecs::Entity> GetEntity(std::string const &name) const;

	// Get Entity name
	std::string GetEntityName(Entity::Id id) const;

	// Enable the Entity
	void EnableEntity(Entity::Id id);

	// Disable the Entity
	void DisableEntity(Entity::Id id);

	// Check whether the Entity is enabled or not
	bool IsEntityEnabled(Entity::Id id) const;

	// Remove the Entity
	void RemoveEntity(Entity::Id id);

	// Remove all Entities
	void RemoveAllEntities();

	// Check whether an Entity is valid or not
	bool IsEntityValid(Entity::Id id) const;

	// Update the World
	void Update(float elapsed);

	// Clear the World by removing all Systems and Entities
	void Clear();

private:
	struct EntityAttributes
	{
		// Entity
		Entity entity;

		// Is this Entity valid (hasn't been removed)
		bool isValid;

		// Is this Entity enabled
		bool isEnabled;

		// Entity name
		std::optional<std::string> name;

		// The Systems this Entity is attached
		std::vector<detail::TypeId> systems;
	};

	struct EntityAction
	{
		enum class Action
		{
			Enable, Disable, Refresh, Remove
		};

		// Entity ID
		Entity::Id id;

		// Action to perform on this Entity
		Action action;
	};

	enum AttachStatus
	{
		Attached, AlreadyAttached, Detached, NotAttached
	};

	// Refresh the Entity Systems list
	void RefreshEntity(Entity::Id id);

	// Update the Systems
	template<class Func>
	void UpdateSystems(Func &&func);

	// Update the Entities within the World (enable, disable, remove)
	void UpdateEntities();

	// Execute an action
	void ExecuteAction(EntityAction const &action);

	// Add Entity to the Systems it meets the requirements
	void ActionEnable(Entity::Id id);

	// Remove Entity from the Systems it meets the requirements
	void ActionDisable(Entity::Id id);

	// Attach the Entity to the Systems it meets the requirements or detach
	// it from the Systems it does not meet the requirements anymore
	// Used after addComponent and removeComponent
	void ActionRefresh(Entity::Id id);

	// Remove Entity data from the World
	void ActionRemove(Entity::Id id);

	// Checks the requirements the Entity meets for each Systems
	// Used by actionEnable and actionRefresh
	AttachStatus TryAttach(System &system, detail::TypeId systemId, Entity::Id id);

	// Extend the Entity and Component arrays
	void Extend(std::size_t size);

	// List of all Entities
	std::vector<EntityAttributes> m_entities;

	// List of Entities that have been modified
	std::vector<EntityAction> m_actions;

	// List of all Entity names, associated to their Entities, for
	// faster search
	std::unordered_map<std::string, Entity::Id> m_names;

	// List of all Components of all Entities of the World
	detail::ComponentHolder m_components;

	// List of all Systems of the World
	detail::SystemHolder m_systems;

	// List of all System waiting to be started
	std::vector<Reference<System>> m_newSystems;

	// ID Pool
	detail::EntityPool m_pool;

	// Event Dispacher
	EventDispatcher m_evtDispatcher;

	// Only Entity is able to use the detail::ComponentHolder
	friend class Entity;

	// Only System is able to use the EventDispatcher
	friend class System;
};
}
