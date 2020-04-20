#pragma once

#include "Scene.hpp"

namespace acid {
template<typename T>
bool Scene::HasSystem() const {
	return systems.HasSystem<T>();
}

template<typename T>
T *Scene::GetSystem() const {
	return systems.GetSystem<T>();
}

template<typename T, typename... Args>
T *Scene::AddSystem(std::size_t priority, Args &&...args) {
	systems.AddSystem<T>(priority, std::make_unique<T>(std::forward<Args>(args)...));

	auto system = GetSystem<T>();
	newSystems.emplace_back(system);

	// Sets the System Scene.
	system->scene = this;
	return system;
}

template<typename T>
void Scene::RemoveSystem() {
	systems.RemoveSystem<T>();
}
}
