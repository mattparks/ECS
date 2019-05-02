#include "System.inl"

#include <algorithm>
#include "World.hpp"

namespace ecs
{
System::~System()
{
	DisconnectAllEvents();
}

void System::DetachAll()
{
	// Enabled Entities
	for (auto &entity : m_enabledEntities)
	{
		DisableEvent(entity);
		DetachEvent(entity);
	}

	// Disabled Entities
	for (auto &entity : m_disabledEntities)
	{
		DetachEvent(entity);
	}

	m_enabledEntities.clear();
	m_disabledEntities.clear();

	m_status.clear();
}

void System::AttachEntity(Entity const &entity)
{
	if (GetEntityStatus(entity) == EntityStatus::NotAttached)
	{
		// Add Entity to the Disabled list
		// The Entity is not enabled by default
		m_disabledEntities.emplace_back(entity);

		AttachEvent(entity);
		SetEntityStatus(entity, EntityStatus::Disabled);
	}
}

void System::DetachEntity(Entity const &entity)
{
	const auto status = GetEntityStatus(entity);

	if (status != EntityStatus::NotAttached)
	{
		if (status == EntityStatus::Enabled)
		{
			// Remove Entity from Enabled list
			m_enabledEntities.erase(std::remove(m_enabledEntities.begin(), m_enabledEntities.end(), entity), m_enabledEntities.end());
			DisableEvent(entity);
		}
		else
		{
			// Remove Entity from Disabled list
			m_disabledEntities.erase(std::remove(m_disabledEntities.begin(), m_disabledEntities.end(), entity), m_disabledEntities.end());
		}

		DetachEvent(entity);
		SetEntityStatus(entity, EntityStatus::NotAttached);
	}
}

void System::EnableEntity(Entity const &entity)
{
	if (GetEntityStatus(entity) == EntityStatus::Disabled)
	{
		// Remove Entity from Disabled list
		m_disabledEntities.erase(std::remove(m_disabledEntities.begin(), m_disabledEntities.end(), entity), m_disabledEntities.end());

		// Then, add it to the Enabled list
		m_enabledEntities.emplace_back(entity);

		EnableEvent(entity);
		SetEntityStatus(entity, EntityStatus::Enabled);
	}
}

void System::DisableEntity(Entity const &entity)
{
	if (GetEntityStatus(entity) == EntityStatus::Enabled)
	{
		// Remove Entity from Enabled list
		m_enabledEntities.erase(std::remove(m_enabledEntities.begin(), m_enabledEntities.end(), entity), m_enabledEntities.end());

		// Then, add it to the Disabled list
		m_disabledEntities.emplace_back(entity);

		DisableEvent(entity);
		SetEntityStatus(entity, EntityStatus::Disabled);
	}
}

void System::StartEvent()
{
	CallEvent(std::bind(&System::OnStart, this));
}

void System::ShutdownEvent()
{
	CallEvent(std::bind(&System::OnShutdown, this));
}

void System::PreUpdateEvent(float elapsed)
{
	CallEvent(std::bind(&System::OnPreUpdate, this, elapsed));
}

void System::UpdateEvent(float elapsed)
{
	CallEvent(std::bind(&System::OnUpdate, this, elapsed));
}

void System::PostUpdateEvent(float elapsed)
{
	CallEvent(std::bind(&System::OnPostUpdate, this, elapsed));
}

void System::AttachEvent(Entity const &entity)
{
	CallEvent(std::bind(&System::OnEntityAttached, this, entity));
}

void System::DetachEvent(Entity const &entity)
{
	CallEvent(std::bind(&System::OnEntityDetached, this, entity));
}

void System::EnableEvent(Entity const &entity)
{
	CallEvent(std::bind(&System::OnEntityEnabled, this, entity));
}

void System::DisableEvent(Entity const &entity)
{
	CallEvent(std::bind(&System::OnEntityDisabled, this, entity));
}

std::vector<Entity> const &System::GetEntities() const
{
	return m_enabledEntities;
}

std::size_t System::GetEntityCount() const noexcept
{
	return m_enabledEntities.size();
}

World &System::GetWorld()
{
	if (!m_world.has_value())
	{
		throw std::runtime_error("System is not attached to any World");
	}

	return m_world.value();
}

World const &System::GetWorld() const
{
	if (!m_world.has_value())
	{
		throw std::runtime_error("System is not attached to any World");
	}

	return m_world.value();
}

void System::OnStart() { }

void System::OnShutdown() { }

void System::OnPreUpdate(float) { }

void System::OnUpdate(float) { }

void System::OnPostUpdate(float) { }

void System::OnEntityAttached(Entity) { }

void System::OnEntityDetached(Entity) { }

void System::OnEntityEnabled(Entity) { }

void System::OnEntityDisabled(Entity) { }

ComponentFilter &System::GetFilter()
{
	return m_filter;
}

void System::DisconnectEvent(Event::Id id)
{
	if (m_events.find(id) != m_events.end())
	{
		GetWorld().m_evtDispatcher.Clear(id);
		m_events.erase(id);
	}
}

void System::DisconnectAllEvents()
{
	auto &evtDispatcher = GetWorld().m_evtDispatcher;

	for (auto id : m_events)
	{
		evtDispatcher.Clear(id);
	}

	m_events.clear();
}

System::EntityStatus System::GetEntityStatus(Entity::Id id) const
{
	const auto it = m_status.find(id);

	if (it != m_status.end())
	{
		return it->second;
	}

	return EntityStatus::NotAttached;
}

void System::SetEntityStatus(Entity::Id id, EntityStatus status)
{
	if (status == EntityStatus::NotAttached)
	{
		m_status.erase(id);
	}
	else
	{
		m_status[id] = status;
	}
}
}
