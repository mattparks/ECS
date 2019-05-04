#pragma once

#include <memory>
#include "Scene.hpp"

namespace ecs
{
template<typename T>
bool Scene::HasSystem() const
{
	return m_systems.HasSystem<T>();
}

template<typename T>
T &Scene::GetSystem()
{
	return m_systems.GetSystem<T>();
}

template<typename T>
const T &Scene::GetSystem() const
{
	return m_systems.GetSystem<T>();
}

template<typename T, typename... Args>
T &Scene::AddSystem(const std::size_t &priority, Args &&...args)
{
	m_systems.AddSystem<T>(priority, std::make_unique<T>(std::forward<Args>(args)...));
	m_newSystems.emplace_back(GetSystem<T>());

	// Sets the System Scene.
	GetSystem<T>().m_scene = *this;

	return GetSystem<T>();
}

template<typename T>
void Scene::RemoveSystem()
{
	m_systems.RemoveSystem<T>();
}
}
