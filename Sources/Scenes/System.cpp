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
	// Enabled Entities.
	for (auto &entity : m_enabledEntities)
	{
		DisableEvent(entity);
		DetachEvent(entity);
	}

	// Disabled Entities.
	for (auto &entity : m_disabledEntities)
	{
		DetachEvent(entity);
	}

	m_enabledEntities.clear();
	m_disabledEntities.clear();

	m_status.clear();
}

void System::AttachEntity(const Entity &entity)
{
	if (GetEntityStatus(entity) == EntityStatus::NotAttached)
	{
		// Add Entity to the Disabled list.
		// The Entity is not enabled by default.
		m_disabledEntities.emplace_back(entity);

		AttachEvent(entity);
		SetEntityStatus(entity, EntityStatus::Disabled);
	}
}

void System::DetachEntity(const Entity &entity)
{
	const auto status = GetEntityStatus(entity);

	if (status != EntityStatus::NotAttached)
	{
		if (status == EntityStatus::Enabled)
		{
			// Remove Entity from Enabled list.
			m_enabledEntities.erase(std::remove(m_enabledEntities.begin(), m_enabledEntities.end(), entity), m_enabledEntities.end());
			DisableEvent(entity);
		}
		else
		{
			// Remove Entity from Disabled list.
			m_disabledEntities.erase(std::remove(m_disabledEntities.begin(), m_disabledEntities.end(), entity), m_disabledEntities.end());
		}

		DetachEvent(entity);
		SetEntityStatus(entity, EntityStatus::NotAttached);
	}
}

void System::EnableEntity(const Entity &entity)
{
	if (GetEntityStatus(entity) == EntityStatus::Disabled)
	{
		// Remove Entity from Disabled list.
		m_disabledEntities.erase(std::remove(m_disabledEntities.begin(), m_disabledEntities.end(), entity), m_disabledEntities.end());

		// Then, add it to the Enabled list.
		m_enabledEntities.emplace_back(entity);

		EnableEvent(entity);
		SetEntityStatus(entity, EntityStatus::Enabled);
	}
}

void System::DisableEntity(const Entity &entity)
{
	if (GetEntityStatus(entity) == EntityStatus::Enabled)
	{
		// Remove Entity from Enabled list.
		m_enabledEntities.erase(std::remove(m_enabledEntities.begin(), m_enabledEntities.end(), entity), m_enabledEntities.end());

		// Then, add it to the Disabled list.
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

void System::PreUpdateEvent(const float &delta)
{
	CallEvent(std::bind(&System::OnPreUpdate, this, delta));
}

void System::UpdateEvent(const float &delta)
{
	CallEvent(std::bind(&System::OnUpdate, this, delta));
}

void System::PostUpdateEvent(const float &delta)
{
	CallEvent(std::bind(&System::OnPostUpdate, this, delta));
}

void System::AttachEvent(const Entity &entity)
{
	CallEvent(std::bind(&System::OnEntityAttached, this, entity));
}

void System::DetachEvent(const Entity &entity)
{
	CallEvent(std::bind(&System::OnEntityDetached, this, entity));
}

void System::EnableEvent(const Entity &entity)
{
	CallEvent(std::bind(&System::OnEntityEnabled, this, entity));
}

void System::DisableEvent(const Entity &entity)
{
	CallEvent(std::bind(&System::OnEntityDisabled, this, entity));
}

const std::vector<Entity> &System::GetEntities() const
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

const World &System::GetWorld() const
{
	if (!m_world.has_value())
	{
		throw std::runtime_error("System is not attached to any World");
	}

	return m_world.value();
}

void System::OnStart() { }

void System::OnShutdown() { }

void System::OnPreUpdate(const float &delta) { }

void System::OnUpdate(const float &delta) { }

void System::OnPostUpdate(const float &delta) { }

void System::OnEntityAttached(Entity entity) { }

void System::OnEntityDetached(Entity entity) { }

void System::OnEntityEnabled(Entity entity) { }

void System::OnEntityDisabled(Entity entity) { }

ComponentFilter &System::GetFilter()
{
	return m_filter;
}

void System::DisconnectEvent(const Event::Id &id)
{
	if (m_events.find(id) != m_events.end())
	{
		GetWorld().m_eventDispatcher.Clear(id);
		m_events.erase(id);
	}
}

void System::DisconnectAllEvents()
{
	auto &evtDispatcher = GetWorld().m_eventDispatcher;

	for (auto id : m_events)
	{
		evtDispatcher.Clear(id);
	}

	m_events.clear();
}

System::EntityStatus System::GetEntityStatus(const Entity::Id &id) const
{
	const auto it = m_status.find(id);

	if (it != m_status.end())
	{
		return it->second;
	}

	return EntityStatus::NotAttached;
}

void System::SetEntityStatus(const Entity::Id &id, const EntityStatus &status)
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
