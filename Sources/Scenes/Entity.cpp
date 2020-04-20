#include "Entity.inl"

#include "Scene.hpp"

namespace acid {
Entity::Entity(Id id, Scene *scene) :
	id(id),
	scene(scene) {
}

void Entity::RemoveAllComponents() {
	scene->components.RemoveAllComponents(id);
	scene->RefreshEntity(id);
}

std::string Entity::GetName() const {
	return scene->GetEntityName(id);
}

bool Entity::IsEnabled() const {
	return scene->IsEntityEnabled(id);
}

void Entity::Enable() {
	scene->EnableEntity(id);
}

void Entity::Disable() {
	scene->DisableEntity(id);
}

bool Entity::IsValid() const {
	return scene && scene->IsEntityValid(id);
}

void Entity::Remove() {
	scene->RemoveEntity(id);
}

bool Entity::operator==(const Entity &other) const {
	return id == other.id && scene == other.scene;
}

bool Entity::operator!=(const Entity &other) const {
	return !(*this == other);
}
}
