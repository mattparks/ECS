#pragma once

#include <memory>
#include <utility>
#include "World.hpp"

namespace ecs
{
template<class T, class... Args>
T &World::AddSystem(std::size_t priority, Args &&...args)
{
	m_systems.AddSystem<T>(priority, std::make_unique<T>(std::forward<Args>(args)...));

	m_newSystems.emplace_back(GetSystem<T>());

	// Set System's World
	GetSystem<T>().m_world = *this;

	return GetSystem<T>();
}

template<class T>
T &World::GetSystem()
{
	return m_systems.GetSystem<T>();
}

template<class T>
T const &World::GetSystem() const
{
	return m_systems.GetSystem<T>();
}

template<class T>
bool World::HasSystem() const
{
	return m_systems.HasSystem<T>();
}

template<class T>
void World::RemoveSystem()
{
	m_systems.RemoveSystem<T>();
}

template<class Func>
void World::UpdateSystems(Func &&func)
{
	UpdateEntities();

	m_systems.ForEach(std::forward<Func>(func));
}
}
