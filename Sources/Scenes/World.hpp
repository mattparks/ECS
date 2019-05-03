#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "Helpers/NonCopyable.hpp"
#include "Helpers/Reference.hpp"
#include "Helpers/TypeInfo.hpp"
#include "Detail/ComponentHolder.hpp"
#include "Detail/EntityPool.hpp"
#include "Detail/SystemHolder.hpp"
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

	~World();

	/**
	 * Adds a System.
	 * @tparam T The System type.
	 * @tparam Args The constructor arg types.
	 * @param priority The System priority.
	 * @param args The constructor args.
	 * @return The System.
	 */
	template<typename T, typename... Args>
	T &AddSystem(const std::size_t &priority = 0, Args &&...args);

	/**
	 * Gets a System.
	 * @tparam T The System type.
	 * @return The System.
	 */
	template<typename T>
	T &GetSystem();

	/**
	 * Gets a System.
	 * @tparam T The System type.
	 * @return The System.
	 */
	template<typename T>
	const T &GetSystem() const;

	/**
	 * Checks whether a System exists or not.
	 * @tparam T The System type.
	 * @return If the World has the System.
	 */
	template<typename T>
	bool HasSystem() const;

	/**
	 * Removes a System.
	 * @tparam T The System type.
	 */
	template<typename T>
	void RemoveSystem();

	/**
	 * Removes all Systems.
	 */
	void RemoveAllSystems();

	/**
	 * Creates a new Entity.
	 * @return The Entity.
	 */
	Entity CreateEntity();

	/**
	 * Creates a new named Entity.
	 * @param name The Entity name.
	 * @return The Entity.
	 */
	Entity CreateEntity(const std::string &name);

	/**
	 * Gets a Entity by ID.
	 * @param id The Entity ID.
	 * @return The entity.
	 */
	std::optional<Entity> GetEntity(const Entity::Id &id) const;

	/**
	 * Gets a Entity by name.
	 * @param name The Entity name.
	 * @return The entity.
	 */
	std::optional<Entity> GetEntity(const std::string &name) const;

	/**
	 * Gets a Entity name.
	 * @param id The Entity ID.
	 * @return The Entity name.
	 */
	std::string GetEntityName(const Entity::Id &id) const;

	/**
	 * Gets whether the Entity is enabled or not.
	 * @param id The Entity ID.
	 * @return If the Entity is enabled.
	 */
	bool IsEntityEnabled(const Entity::Id &id) const;

	/**
	 * Enables a Entity.
	 * @param id The Entity ID.
	 */
	void EnableEntity(const Entity::Id &id);

	/**
	 * Disables a Entity.
	 * @param id The Entity ID.
	 */
	void DisableEntity(const Entity::Id &id);

	/**
	 * Gets whether an Entity is valid or not.
	 * @param id The Entity ID.
	 * @return If the Entity is valid.
	 */
	bool IsEntityValid(const Entity::Id &id) const;

	/**
	 * Removes a Entity.
	 * @param id The Entity ID.
	 */
	void RemoveEntity(const Entity::Id &id);

	/**
	 * Removes all Entities.
	 */
	void RemoveAllEntities();

	/**
	 * Updates the World.
	 * @param delta The time delta between the last update.
	 */
	void Update(const float &delta);

	/**
	 * Clears the World by removing all Systems and Entities.
	 */
	void Clear();

private:
	class EntityAttributes
	{
	public:
		// Entity.
		Entity m_entity;

		// Is this Entity enabled.
		bool m_enabled = true;

		// Is this Entity valid (hasn't been removed).
		bool m_valid = true;

		// Entity name.
		std::optional<std::string> m_name;

		// The Systems this Entity is attached.
		std::vector<TypeId> m_systems;
	};

	enum EntityAttachStatus
	{
		Attached, AlreadyAttached, Detached, NotAttached
	};

	/**
	 * Runs updates on all Systems.
	 * @tparam Func The function type.
	 * @param func The function.
	 */
	template<typename Func>
	void UpdateSystems(Func &&func);

	/**
	 * Extends the Entity and Component arrays.
	 * @param size The new size.
	 */
	void Extend(const std::size_t &size);

	/**
	 * Refreshes the Entity and Systems list.
	 * @param id The Entity ID.
	 */
	void RefreshEntity(const Entity::Id &id);

	/**
	 * Checks the requirements the Entity meets for each Systems.
	 * @param system The System.
	 * @param systemId The System ID.
	 * @param id The Entity ID.
	 * @return The attachment status.
	 */
	EntityAttachStatus TryEntityAttach(System &system, const TypeId &systemId, const Entity::Id &id);

	friend class Entity;
	friend class System;

	// List of all Entities.
	std::vector<EntityAttributes> m_entities;

	// List of all Entity names, associated to their Entities, for faster search.
	std::unordered_map<std::string, Entity::Id> m_names;

	// List of all Components of all Entities of the World.
	ComponentHolder m_components;

	// List of all Systems of the World.
	SystemHolder m_systems;

	// List of all System waiting to be started.
	std::vector<Reference<System>> m_newSystems;

	// Entity ID Pool.
	EntityPool m_pool;

	// Event Dispacher.
	EventDispatcher m_eventDispatcher;
};
}
