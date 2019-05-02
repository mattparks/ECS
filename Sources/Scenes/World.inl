#pragma once

#include <memory>
#include <utility>
#include "World.hpp"

template<class T, class... Args>
T &ecs::World::AddSystem(std::size_t priority, Args &&...args)
{
	m_systems.AddSystem<T>(priority, std::make_unique<T>(std::forward<Args>(args)...));

	m_newSystems.emplace_back(GetSystem<T>());

	// Set System's World
	GetSystem<T>().m_world = *this;

	return GetSystem<T>();
}

template<class T>
T &ecs::World::GetSystem()
{
	return m_systems.GetSystem<T>();
}

template<class T>
T const &ecs::World::GetSystem() const
{
	return m_systems.GetSystem<T>();
}

template<class T>
bool ecs::World::HasSystem() const
{
	return m_systems.HasSystem<T>();
}

template<class T>
void ecs::World::RemoveSystem()
{
	m_systems.RemoveSystem<T>();
}

template<class Func>
void ecs::World::UpdateSystems(Func &&func)
{
	UpdateEntities();

	m_systems.ForEach(std::forward<Func>(func));
}
