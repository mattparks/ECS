#include "Scene.hpp"

#include <iostream>

#include "Entity.inl"

namespace acid {
Scene::Scene(std::unique_ptr<Camera> &&camera) :
	camera(std::move(camera)) {
}

Scene::~Scene() {
	Clear();
}

void Scene::RemoveAllSystems() {
	systems.RemoveAllSystems();
}

Entity Scene::CreateEntity() {
	const auto id = pool.Create();

	// Resize containers if necessary.
	Extend(id + 1);

	entities[id].entity = Entity(id, this);
	entities[id].enabled = true;
	entities[id].valid = true;

	EnableEntity(entities[id].entity);

	return entities[id].entity;
}

Entity Scene::CreateEntity(const std::string &name) {
	if (names.find(name) != names.end()) {
		throw std::runtime_error("Entity name already in use");
	}

	const auto entity = CreateEntity();

	names[name] = entity.GetId();
	entities[entity.GetId()].name = name;

	return entity;
}

Entity Scene::CreatePrefabEntity(const std::string &filename) {
	auto entity = CreateEntity();
	// TODO
	//auto prefab = EntityPrefab(filename);
	//prefab >> entity;
	return entity;
}

std::optional<Entity> Scene::GetEntity(Entity::Id id) const {
	if (!IsEntityValid(id)) {
		return std::nullopt;
	}

	return entities[id].entity;
}

std::optional<Entity> Scene::GetEntity(const std::string &name) const {
	const auto it = names.find(name);

	if (it == names.end()) {
		return std::nullopt;
	}

	return GetEntity(it->second);
}

std::string Scene::GetEntityName(Entity::Id id) const {
	if (!IsEntityValid(id)) {
		throw std::runtime_error("Entity ID is not valid");
	}

	if (entities[id].name.has_value()) {
		return entities[id].name.value();
	}

	return {};
}

bool Scene::IsEntityEnabled(Entity::Id id) const {
	return IsEntityValid(id) && entities[id].enabled;
}

void Scene::EnableEntity(Entity::Id id) {
	if (!IsEntityValid(id)) {
		throw std::runtime_error("Entity ID is not valid");
	}

	actions.emplace_back(EntityAction(id, EntityAction::Action::Enable));
}

void Scene::DisableEntity(Entity::Id id) {
	if (!IsEntityValid(id)) {
		throw std::runtime_error("Entity ID is not valid");
	}

	actions.emplace_back(EntityAction(id, EntityAction::Action::Disable));
}

bool Scene::IsEntityValid(Entity::Id id) const {
	return id < entities.size() && entities[id].valid;
}

void Scene::RemoveEntity(Entity::Id id) {
	if (!IsEntityValid(id)) {
		throw std::runtime_error("Entity ID is not valid");
	}

	actions.emplace_back(EntityAction(id, EntityAction::Action::Remove));
}

void Scene::RefreshEntity(Entity::Id id) {
	if (!IsEntityValid(id)) {
		throw std::runtime_error("Entity ID is not valid");
	}

	actions.emplace_back(EntityAction(id, EntityAction::Action::Refresh));
}

void Scene::RemoveAllEntities() {
	for (const auto &entity : entities) {
		// We may iterate through invalid entities.
		if (entity.valid) {
			RemoveEntity(entity.entity.GetId());
		}
	}
}

void Scene::Update(float delta) {
	// Start new Systems
	for (auto &system : newSystems) {
		system->OnStart();
	}

	newSystems.clear();

	UpdateEntities();
	systems.ForEach([delta](System &system, TypeId) {
		system.Update(delta);
	});
}

void Scene::Clear() {
	RemoveAllSystems();

	entities.clear();
	actions.clear();
	names.clear();

	components.Clear();
	pool.Reset();
}

void Scene::UpdateEntities() {
	// Here, we copy actions to make possible to create, enable, etc.
	// Entities within event handlers like system::onEntityAttached, etc.
	const auto actionsList = std::move(actions);
	actions = decltype(actions)();

	for (const auto &action : actionsList) {
		try {
			ExecuteAction(action);
		} catch (const std::exception & e) {
			std::cout << e.what() << '\n';
		}
	}
}

void Scene::ExecuteAction(const EntityAction &action) {
	if (!IsEntityValid(action.id)) {
		throw std::runtime_error("Entity action ID is not valid");
	}

	switch (action.action) {
	case EntityAction::Action::Enable:
		ActionEnable(action.id);
		break;
	case EntityAction::Action::Disable:
		ActionDisable(action.id);
		break;
	case EntityAction::Action::Remove:
		ActionRemove(action.id);
		break;
	case EntityAction::Action::Refresh:
		ActionRefresh(action.id);
		break;
	}
}

void Scene::ActionEnable(Entity::Id id) {
	systems.ForEach([&](System &system, TypeId systemId) {
		const auto attachStatus = TryEntityAttach(system, systemId, id);

		if (attachStatus == EntityAttachStatus::AlreadyAttached || attachStatus == EntityAttachStatus::Attached) {
			// The Entity is attached to the System, it is enabled.
			system.EnableEntity(entities[id].entity);
		}
	});
}

void Scene::ActionDisable(Entity::Id id) {
	entities[id].enabled = false;

	systems.ForEach([&](System &system, TypeId systemId) {
		// Is the Entity attached to the System?
		if (systemId < entities[id].systems.size() && entities[id].systems[systemId]) {
			system.DisableEntity(entities[id].entity);
		}
	});
}

void Scene::ActionRemove(Entity::Id id) {
	systems.ForEach([&](System &system, TypeId systemId) {
		// Is the Entity attached to the System?
		if (systemId < entities[id].systems.size() && entities[id].systems[systemId]) {
			system.DetachEntity(entities[id].entity);
			entities[id].systems[systemId] = false;
		}
	});

	// Invalidate the Entity and reset its attributes.
	entities[id].valid = false;
	entities[id].systems.clear();

	// Remove its name from the list
	if (entities[id].name.has_value()) {
		names.erase(entities[id].name.value());
		entities[id].name.reset();
	}

	components.RemoveAllComponents(id);
	pool.Store(id);
}

void Scene::ActionRefresh(Entity::Id id) {
	systems.ForEach([&](System &system, TypeId systemId) {
		const auto attachStatus = TryEntityAttach(system, systemId, id);

		if (entities[id].enabled && attachStatus == EntityAttachStatus::Attached) {
			// If the Entity has been attached and is enabled, enable it into the System.
			system.EnableEntity(entities[id].entity);
		}
	});
}

void Scene::Extend(std::size_t size) {
	if (size > entities.size()) {
		entities.resize(size);
		components.Resize(size);
	}
}

Scene::EntityAttachStatus Scene::TryEntityAttach(System &system, TypeId systemId, Entity::Id id) {
	// Does the Entity match the requirements to be part of the System?
	if (system.GetFilter().Check(components.GetComponentsMask(id))) {
		// Is the Entity not already attached to the System?
		if (systemId >= entities[id].systems.size() || !entities[id].systems[systemId]) {
			if (systemId >= entities[id].systems.size()) {
				entities[id].systems.resize(systemId + 1, false);
			}

			entities[id].systems[systemId] = true;
			system.AttachEntity(entities[id].entity);

			// The Entity has been attached to the System.
			return EntityAttachStatus::Attached;
		}

		// Otherwise, if the Entity is already attached to the System.
		return EntityAttachStatus::AlreadyAttached;
	}

	// If the Entity is already attached to the System but doest not match the requirements anymore, we detach it from the System.
	if (systemId < entities[id].systems.size() && entities[id].systems[systemId]) {
		system.DetachEntity(entities[id].entity);
		entities[id].systems[systemId] = false;

		// The Entity has been detached from the System.
		return EntityAttachStatus::Detached;
	}

	// Nothing happened because the Entity is not attached to the System and does not match the requirements to be part of it.
	return EntityAttachStatus::NotAttached;
}
}
