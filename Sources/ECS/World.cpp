#include "World.hpp"
#include "World.inl"

#include <stdexcept>
#include "Entity.hpp"
#include "Exceptions/Exception.hpp"
#include "Exceptions/InvalidEntity.hpp"
#include "Log.hpp"
#include "Entity.inl"

namespace ecs
{
	World::~World()
	{
		Clear();
	}

	void World::RemoveAllSystems()
	{
		m_systems.RemoveAllSystems();
	}

	Entity World::CreateEntity()
	{
		const auto id = m_pool.Create();

		// Resize containers if necessary
		Extend(id + 1);

		// Entity
		m_entities[id].entity = Entity(id, *this);

		// Attributes
		m_entities[id].isValid = true;
		m_entities[id].isEnabled = true;

		EnableEntity(m_entities[id].entity);

		return m_entities[id].entity;
	}

	Entity World::CreateEntity(std::string const& name)
	{
		if (m_names.find(name) != m_names.end())
		{
			throw Exception("Entity name already used.", "World::CreateEntity()");
		}

		const auto entity = CreateEntity();

		m_names[name] = entity.GetId();
		m_entities[entity.GetId()].name = name;

		return entity;
	}

	std::optional<Entity> World::GetEntity(Entity::Id id) const
	{
		if (!IsEntityValid(id))
		{
			return std::nullopt;
		}

		return m_entities[id].entity;
	}

	std::optional<Entity> World::GetEntity(std::string const& name) const
	{
		const auto it = m_names.find(name);

		if (it == m_names.end())
		{
			return std::nullopt;
		}

		return GetEntity(it->second);
	}

	std::string World::GetEntityName(Entity::Id id) const
	{
		if (!IsEntityValid(id))
		{
			throw InvalidEntity("World::GetEntityName()");
		}

		if (m_entities[id].name.has_value())
		{
			return m_entities[id].name.value();
		}

		return {};
	}

	void World::EnableEntity(Entity::Id id)
	{
		if (!IsEntityValid(id))
		{
			throw InvalidEntity("World::EnableEntity()");
		}

		m_actions.emplace_back(EntityAction{ id, EntityAction::Action::Enable });
	}

	void World::DisableEntity(Entity::Id id)
	{
		if (!IsEntityValid(id))
		{
			throw InvalidEntity("World::DisableEntity()");
		}

		m_actions.emplace_back(EntityAction{ id, EntityAction::Action::Disable });
	}

	void World::RefreshEntity(Entity::Id id)
	{
		if (!IsEntityValid(id))
		{
			throw InvalidEntity("World::RefreshEntity()");
		}

		m_actions.emplace_back(EntityAction{ id, EntityAction::Action::Refresh });
	}

	bool World::IsEntityEnabled(Entity::Id id) const
	{
		return IsEntityValid(id) && m_entities[id].isEnabled;
	}

	void World::RemoveEntity(Entity::Id id)
	{
		if (!IsEntityValid(id))
		{
			throw InvalidEntity("World::RemoveEntity()");
		}

		m_actions.emplace_back(EntityAction{ id, EntityAction::Action::Remove });
	}

	void World::RemoveAllEntities()
	{
		for (const auto& entity : m_entities)
		{
			// We may iterate through invalid entities
			if (entity.isValid)
			{
				RemoveEntity(entity.entity.GetId());
			}
		}
	}

	bool World::IsEntityValid(Entity::Id id) const
	{
		return id < m_entities.size() && m_entities[id].isValid;
	}

	void World::Update(float elapsed)
	{
		// Start new Systems
		for (auto& system : m_newSystems)
		{
			system->StartEvent();
		}

		m_newSystems.clear();

		UpdateSystems([elapsed](System & system, detail::TypeId)
			{
				system.PreUpdateEvent(elapsed);
			});

		UpdateSystems([elapsed](System & system, detail::TypeId)
			{
				system.UpdateEvent(elapsed);
			});

		UpdateSystems([elapsed](System & system, detail::TypeId)
			{
				system.PostUpdateEvent(elapsed);
			});
	}

	void World::Clear()
	{
		RemoveAllSystems();

		m_entities.clear();
		m_actions.clear();
		m_names.clear();

		m_evtDispatcher.ClearAll();
		m_components.Clear();
		m_pool.Reset();
	}

	void World::UpdateEntities()
	{
		// Here, we copy m_actions to make possible to create, enable, etc.
		// Entities within event handlers like system::onEntityAttached, etc.
		const auto actionsList = m_actions;
		m_actions.clear();

		for (const auto& action : actionsList)
		{
			try
			{
				ExecuteAction(action);
			}
			catch (std::exception const& e)
			{
				Log::Error(e.what());
			}
		}
	}

	void World::ExecuteAction(EntityAction const& action)
	{
		if (!IsEntityValid(action.id))
		{
			throw InvalidEntity("World::ExecuteAction()");
		}

		switch (action.action)
		{
		case EntityAction::Action::Enable:
			ActionEnable(action.id);
			break;
		case EntityAction::Action::Disable:
			ActionDisable(action.id);
			break;
		case EntityAction::Action::Refresh:
			ActionRefresh(action.id);
			break;
		case EntityAction::Action::Remove:
			ActionRemove(action.id);
			break;
		}
	}

	void World::ActionEnable(Entity::Id id)
	{
		m_systems.ForEach([&](System & system, detail::TypeId systemId)
			{
				const auto status = TryAttach(system, systemId, id);

				if (status == AttachStatus::AlreadyAttached || status == AttachStatus::Attached)
				{
					// The Entity is attached to the System, we enable it
					system.EnableEntity(m_entities[id].entity);
				}
			});
	}

	void World::ActionDisable(Entity::Id id)
	{
		m_entities[id].isEnabled = false;

		m_systems.ForEach([&](System & system, detail::TypeId systemId)
			{
				// Is the Entity attached to the System ?
				if (systemId < m_entities[id].systems.size() && m_entities[id].systems[systemId])
				{
					system.DisableEntity(m_entities[id].entity);
				}
			});
	}

	void World::ActionRefresh(Entity::Id id)
	{
		m_systems.ForEach([&](System & system, detail::TypeId systemId)
			{
				const auto status = TryAttach(system, systemId, id);

				if (m_entities[id].isEnabled && status == AttachStatus::Attached)
				{
					// If the Entity has been attached and is enabled,
					// we enable it into the System
					system.EnableEntity(m_entities[id].entity);
				}
			});
	}

	void World::ActionRemove(Entity::Id id)
	{
		m_systems.ForEach([&](System & system, detail::TypeId systemId)
			{
				// Is the Entity attached to the System ?
				if (systemId < m_entities[id].systems.size() && m_entities[id].systems[systemId])
				{
					system.DetachEntity(m_entities[id].entity);
					m_entities[id].systems[systemId] = false;
				}
			});

		// Invalidate the Entity and reset its attributes
		m_entities[id].isValid = false;
		m_entities[id].systems.clear();

		// Remove its name from the list
		if (m_entities[id].name.has_value())
		{
			m_names.erase(m_entities[id].name.value());
			m_entities[id].name.reset();
		}

		m_components.RemoveAllComponents(id);
		m_pool.Store(id);
	}

	World::AttachStatus World::TryAttach(System & system, detail::TypeId systemId, Entity::Id id)
	{
		// Does the Entity match the requirements to be part of the System ?
		if (system.GetFilter().Check(m_components.GetComponentsMask(id)))
		{
			// Is the Entity not already attached to the System ?
			if (systemId >= m_entities[id].systems.size() || !m_entities[id].systems[systemId])
			{
				if (systemId >= m_entities[id].systems.size())
				{
					m_entities[id].systems.resize(systemId + 1, false);
				}

				m_entities[id].systems[systemId] = true;
				system.AttachEntity(m_entities[id].entity);

				// The Entity has been attached to the System
				return AttachStatus::Attached;
			}

			// Otherwise, if the Entity is already attached to the System
			return AttachStatus::AlreadyAttached;
		}
		// If the Entity is already attached to the System but doest not
		// match the requirements anymore, we detach it from the System
		else if (systemId < m_entities[id].systems.size() && m_entities[id].systems[systemId])
		{
			system.DetachEntity(m_entities[id].entity);
			m_entities[id].systems[systemId] = false;

			// The Entity has been detached from the System
			return AttachStatus::Detached;
		}

		// Nothing happened because the Entity is not attached to the System
		// and does not match the requirements to be part of it
		return AttachStatus::NotAttached;
	}

	void World::Extend(std::size_t size)
	{
		if (size > m_entities.size())
		{
			m_entities.resize(size);
			m_components.Resize(size);
		}
	}
}
