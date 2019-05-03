#include "World.inl"

#include <exception>
#include "Engine/Log.hpp"
#include "Entity.hpp"
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

	// Resize containers if necessary.
	Extend(id + 1);

	m_entities[id].m_entity = Entity(id, *this);
	m_entities[id].m_enabled = true;
	m_entities[id].m_valid = true;

	EnableEntity(m_entities[id].m_entity);

	return m_entities[id].m_entity;
}

Entity World::CreateEntity(const std::string &name)
{
	if (m_names.find(name) != m_names.end())
	{
		throw std::runtime_error("Entity name already in use");
	}

	const auto entity = CreateEntity();

	m_names[name] = entity.GetId();
	m_entities[entity.GetId()].m_name = name;

	return entity;
}

std::optional<Entity> World::GetEntity(const Entity::Id &id) const
{
	if (!IsEntityValid(id))
	{
		return std::nullopt;
	}

	return m_entities[id].m_entity;
}

std::optional<Entity> World::GetEntity(const std::string &name) const
{
	const auto it = m_names.find(name);

	if (it == m_names.end())
	{
		return std::nullopt;
	}

	return GetEntity(it->second);
}

std::string World::GetEntityName(const Entity::Id &id) const
{
	if (!IsEntityValid(id))
	{
		throw std::runtime_error("Entity ID is not valid");
	}

	if (m_entities[id].m_name.has_value())
	{
		return m_entities[id].m_name.value();
	}

	return {};
}

bool World::IsEntityEnabled(const Entity::Id &id) const
{
	return IsEntityValid(id) && m_entities[id].m_enabled;
}

void World::EnableEntity(const Entity::Id &id)
{
	if (!IsEntityValid(id))
	{
		throw std::runtime_error("Entity ID is not valid");
	}

	m_systems.ForEach([&](System &system, TypeId systemId)
	{
		const auto attachStatus = TryEntityAttach(system, systemId, id);

		if (attachStatus == EntityAttachStatus::AlreadyAttached || attachStatus == EntityAttachStatus::Attached)
		{
			// The Entity is attached to the System, we enable it.
			system.EnableEntity(m_entities[id].m_entity);
		}
	});
}

void World::DisableEntity(const Entity::Id &id)
{
	if (!IsEntityValid(id))
	{
		throw std::runtime_error("Entity ID is not valid");
	}

	m_entities[id].m_enabled = false;

	m_systems.ForEach([&](System &system, TypeId systemId)
	{
		// Is the Entity attached to the System?
		if (systemId < m_entities[id].m_systems.size() && m_entities[id].m_systems[systemId])
		{
			system.DisableEntity(m_entities[id].m_entity);
		}
	});
}

bool World::IsEntityValid(const Entity::Id &id) const
{
	return id < m_entities.size() && m_entities[id].m_valid;
}

void World::RemoveEntity(const Entity::Id &id)
{
	if (!IsEntityValid(id))
	{
		throw std::runtime_error("Entity ID is not valid");
	}

	m_systems.ForEach([&](System &system, TypeId systemId)
	{
		// Is the Entity attached to the System?
		if (systemId < m_entities[id].m_systems.size() && m_entities[id].m_systems[systemId])
		{
			system.DetachEntity(m_entities[id].m_entity);
			m_entities[id].m_systems[systemId] = false;
		}
	});

	// Invalidate the Entity and reset its attributes.
	m_entities[id].m_valid = false;
	m_entities[id].m_systems.clear();

	// Remove its name from the list
	if (m_entities[id].m_name.has_value())
	{
		m_names.erase(m_entities[id].m_name.value());
		m_entities[id].m_name.reset();
	}

	m_components.RemoveAllComponents(id);
	m_pool.Store(id);
}

void World::RemoveAllEntities()
{
	for (const auto &entity : m_entities)
	{
		// We may iterate through invalid entities.
		if (entity.m_valid)
		{
			RemoveEntity(entity.m_entity.GetId());
		}
	}
}

void World::Update(const float &delta)
{
	// Start new Systems
	for (auto &system : m_newSystems)
	{
		system->StartEvent();
	}

	m_newSystems.clear();

	UpdateSystems([delta](System &system, TypeId)
	{
		system.PreUpdateEvent(delta);
	});

	UpdateSystems([delta](System &system, TypeId)
	{
		system.UpdateEvent(delta);
	});

	UpdateSystems([delta](System &system, TypeId)
	{
		system.PostUpdateEvent(delta);
	});
}

void World::Clear()
{
	RemoveAllSystems();

	m_entities.clear();
	m_names.clear();

	m_eventDispatcher.ClearAll();
	m_components.Clear();
	m_pool.Reset();
}

void World::Extend(const std::size_t &size)
{
	if (size > m_entities.size())
	{
		m_entities.resize(size);
		m_components.Resize(size);
	}
}

void World::RefreshEntity(const Entity::Id &id)
{
	if (!IsEntityValid(id))
	{
		throw std::runtime_error("Entity ID is not valid");
	}

	m_systems.ForEach([&](System &system, TypeId systemId)
	{
		const auto attachStatus = TryEntityAttach(system, systemId, id);

		if (m_entities[id].m_enabled && attachStatus == EntityAttachStatus::Attached)
		{
			// If the Entity has been attached and is enabled, enable it into the System.
			system.EnableEntity(m_entities[id].m_entity);
		}
	});
}

World::EntityAttachStatus World::TryEntityAttach(System &system, const TypeId &systemId, const Entity::Id &id)
{
	// Does the Entity match the requirements to be part of the System?
	if (system.GetFilter().Check(m_components.GetComponentsMask(id)))
	{
		// Is the Entity not already attached to the System?
		if (systemId >= m_entities[id].m_systems.size() || !m_entities[id].m_systems[systemId])
		{
			if (systemId >= m_entities[id].m_systems.size())
			{
				m_entities[id].m_systems.resize(systemId + 1, false);
			}

			m_entities[id].m_systems[systemId] = true;
			system.AttachEntity(m_entities[id].m_entity);

			// The Entity has been attached to the System.
			return Attached;
		}

		// Otherwise, if the Entity is already attached to the System.
		return AlreadyAttached;
	}

	// If the Entity is already attached to the System but doest not match the requirements anymore, we detach it from the System.
	if (systemId < m_entities[id].m_systems.size() && m_entities[id].m_systems[systemId])
	{
		system.DetachEntity(m_entities[id].m_entity);
		m_entities[id].m_systems[systemId] = false;

		// The Entity has been detached from the System.
		return Detached;
	}

	// Nothing happened because the Entity is not attached to the System and does not match the requirements to be part of it.
	return NotAttached;
}
}
