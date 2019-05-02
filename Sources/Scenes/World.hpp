#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "Helpers/NonCopyable.hpp"
#include "Helpers/Reference.hpp"
#include "Detail/ComponentHolder.hpp"
#include "Detail/EntityPool.hpp"
#include "Detail/SystemHolder.hpp"
#include "Detail/TypeInfo.hpp"
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
	template<typename T, typename... Args>
	T &AddSystem(const std::size_t &priority = 0, Args &&...args);

	// Get a System
	template<typename T>
	T &GetSystem();

	// Get a System
	template<typename T>
	const T &GetSystem() const;

	// Check whether a System exists or not
	template<typename T>
	bool HasSystem() const;

	// Remove a System
	template<typename T>
	void RemoveSystem();

	// Remove all Systems
	void RemoveAllSystems();

	// Create a new Entity
	Entity CreateEntity();

	// Create a new named Entity
	Entity CreateEntity(const std::string &name);

	// Get Entity by ID
	std::optional<ecs::Entity> GetEntity(const Entity::Id &id) const;

	// Get Entity by name
	std::optional<ecs::Entity> GetEntity(const std::string &name) const;

	// Get Entity name
	std::string GetEntityName(const Entity::Id &id) const;

	// Enable the Entity
	void EnableEntity(const Entity::Id &id);

	// Disable the Entity
	void DisableEntity(const Entity::Id &id);

	// Check whether the Entity is enabled or not
	bool IsEntityEnabled(const Entity::Id &id) const;

	// Remove the Entity
	void RemoveEntity(const Entity::Id &id);

	// Remove all Entities
	void RemoveAllEntities();

	// Check whether an Entity is valid or not
	bool IsEntityValid(const Entity::Id &id) const;

	// Update the World
	void Update(const float & delta);

	// Clear the World by removing all Systems and Entities
	void Clear();

private:
	struct EntityAttributes
	{
		// Entity
		Entity entity;

		// Is this Entity valid (hasn't been removed)
		bool isValid{true};

		// Is this Entity enabled
		bool isEnabled{true};

		// Entity name
		std::optional<std::string> name;

		// The Systems this Entity is attached
		std::vector<TypeId> systems;
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
	void RefreshEntity(const Entity::Id &id);

	// Update the Systems
	template<typename Func>
	void UpdateSystems(Func &&func);

	// Update the Entities within the World (enable, disable, remove)
	void UpdateEntities();

	// Execute an action
	void ExecuteAction(const EntityAction &action);

	// Add Entity to the Systems it meets the requirements
	void ActionEnable(const Entity::Id &id);

	// Remove Entity from the Systems it meets the requirements
	void ActionDisable(const Entity::Id &id);

	// Attach the Entity to the Systems it meets the requirements or detach
	// it from the Systems it does not meet the requirements anymore
	// Used after addComponent and removeComponent
	void ActionRefresh(const Entity::Id &id);

	// Remove Entity data from the World
	void ActionRemove(const Entity::Id &id);

	// Checks the requirements the Entity meets for each Systems
	// Used by actionEnable and actionRefresh
	AttachStatus TryAttach(System &system, const TypeId &systemId, const Entity::Id &id);

	// Extend the Entity and Component arrays
	void Extend(const std::size_t &size);

	// List of all Entities
	std::vector<EntityAttributes> m_entities;

	// List of Entities that have been modified
	std::vector<EntityAction> m_actions;

	// List of all Entity names, associated to their Entities, for
	// faster search
	std::unordered_map<std::string, Entity::Id> m_names;

	// List of all Components of all Entities of the World
	ComponentHolder m_components;

	// List of all Systems of the World
	SystemHolder m_systems;

	// List of all System waiting to be started
	std::vector<Reference<System>> m_newSystems;

	// ID Pool
	EntityPool m_pool;

	// Event Dispacher
	EventDispatcher m_evtDispatcher;

	// Only Entity is able to use the ComponentHolder
	friend class Entity;

	// Only System is able to use the EventDispatcher
	friend class System;
};
}
