#include "System.inl"

#include <algorithm>
#include "Scene.hpp"

namespace ecs
{
void System::DetachAll()
{
	// Enabled Entities.
	for (auto &entity : m_enabledEntities)
	{
		m_onEntityDisable(entity);
		m_onEntityDetach(entity);
	}

	// Disabled Entities.
	for (auto &entity : m_disabledEntities)
	{
		m_onEntityDetach(entity);
	}

	m_enabledEntities.clear();
	m_disabledEntities.clear();

	m_status.clear();
}

void System::AttachEntity(const Entity &entity)
{
	if (GetEntityStatus(entity) == EntityStatus::NotAttached)
	{
		// Add Entity to the Disabled list. The Entity is not enabled by default.
		m_disabledEntities.emplace_back(entity);

		m_onEntityAttach(entity);
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
			m_onEntityDisable(entity);
		}
		else
		{
			// Remove Entity from Disabled list.
			m_disabledEntities.erase(std::remove(m_disabledEntities.begin(), m_disabledEntities.end(), entity), m_disabledEntities.end());
		}

		m_onEntityDetach(entity);
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

		m_onEntityEnable(entity);
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

		m_onEntityDisable(entity);
		SetEntityStatus(entity, EntityStatus::Disabled);
	}
}

Scene &System::GetScene()
{
	if (!m_scene.has_value())
	{
		throw std::runtime_error("System is not attached to any Scene");
	}

	return m_scene.value();
}

const Scene &System::GetScene() const
{
	if (!m_scene.has_value())
	{
		throw std::runtime_error("System is not attached to any Scene");
	}

	return m_scene.value();
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
