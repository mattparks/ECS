#pragma once

#include "Entity.hpp"
#include "Scene.hpp"

namespace acid {
template<typename T>
bool Entity::HasComponent() const {
	return scene->components.HasComponent<T>(id);
}

template<typename T>
T *Entity::GetComponent() const {
	return scene->components.GetComponent<T>(id);
}

template<typename T, typename... Args>
T *Entity::AddComponent(Args &&...args) {
	scene->components.AddComponent<T>(id, std::make_unique<T>(std::forward<Args>(args)...));
	scene->RefreshEntity(id);
	return GetComponent<T>();
}

template<typename T>
T *Entity::AddComponent(std::unique_ptr<T> &&component) {
	scene->components.AddComponent<T>(id, std::move(component));
	scene->RefreshEntity(id);
	return GetComponent<T>();
}

template<typename T>
void Entity::RemoveComponent() {
	scene->components.RemoveComponent<T>(id);
	scene->RefreshEntity(id);
}
}
