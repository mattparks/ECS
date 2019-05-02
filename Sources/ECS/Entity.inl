#pragma once

#include <memory>
#include <utility>
#include "Entity.hpp"
#include "World.hpp"

template<class T, class... Args>
T &ecs::Entity::AddComponent(Args &&...args)
{
	m_world.value()->m_components.AddComponent<T>(m_id, std::make_unique<T>(std::forward<Args>(args)...));
	m_world.value()->RefreshEntity(m_id);

	return GetComponent<T>();
}

template<class T>
T &ecs::Entity::GetComponent()
{
	return m_world.value()->m_components.GetComponent<T>(m_id);
}

template<class T>
T const &ecs::Entity::GetComponent() const
{
	return m_world.value()->m_components.GetComponent<T>(m_id);
}

template<class T>
bool ecs::Entity::HasComponent() const
{
	return m_world.value()->m_components.HasComponent<T>(m_id);
}

template<class T>
void ecs::Entity::RemoveComponent()
{
	m_world.value()->m_components.RemoveComponent<T>(m_id);
	m_world.value()->RefreshEntity(m_id);
}
