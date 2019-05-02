#pragma once

#include <memory>
#include <utility>
#include "Entity.hpp"
#include "World.hpp"

namespace ecs
{
template<typename T, typename... Args>
T &Entity::AddComponent(Args &&...args)
{
	m_world.value()->m_components.AddComponent<T>(m_id, std::make_unique<T>(std::forward<Args>(args)...));
	m_world.value()->RefreshEntity(m_id);

	return GetComponent<T>();
}

template<typename T>
T &Entity::GetComponent()
{
	return m_world.value()->m_components.GetComponent<T>(m_id);
}

template<typename T>
const T &Entity::GetComponent() const
{
	return m_world.value()->m_components.GetComponent<T>(m_id);
}

template<typename T>
bool Entity::HasComponent() const
{
	return m_world.value()->m_components.HasComponent<T>(m_id);
}

template<typename T>
void Entity::RemoveComponent()
{
	m_world.value()->m_components.RemoveComponent<T>(m_id);
	m_world.value()->RefreshEntity(m_id);
}
}
