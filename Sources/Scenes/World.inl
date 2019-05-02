#pragma once

#include <memory>
#include "World.hpp"

namespace ecs
{
template<typename T, typename... Args>
T &World::AddSystem(const std::size_t &priority, Args &&...args)
{
	m_systems.AddSystem<T>(priority, std::make_unique<T>(std::forward<Args>(args)...));

	m_newSystems.emplace_back(GetSystem<T>());

	// Set System's World
	GetSystem<T>().m_world = *this;

	return GetSystem<T>();
}

template<typename T>
T &World::GetSystem()
{
	return m_systems.GetSystem<T>();
}

template<typename T>
const T &World::GetSystem() const
{
	return m_systems.GetSystem<T>();
}

template<typename T>
bool World::HasSystem() const
{
	return m_systems.HasSystem<T>();
}

template<typename T>
void World::RemoveSystem()
{
	m_systems.RemoveSystem<T>();
}

template<typename Func>
void World::UpdateSystems(Func &&func)
{
	UpdateEntities();
	m_systems.ForEach(std::forward<Func>(func));
}
}
