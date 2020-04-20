#include "System.inl"

#include "Scene.hpp"
#include "System.hpp"

namespace acid {
void System::DetachAll() {
	// Enabled Entities.
	for (auto &entity : enabledEntities) {
		OnEntityDisable(entity);
		OnEntityDetach(entity);
	}

	// Disabled Entities.
	for (auto &entity : disabledEntities) {
		OnEntityDetach(entity);
	}

	enabledEntities.clear();
	disabledEntities.clear();

	status.clear();
}

void System::AttachEntity(const Entity &entity) {
	if (GetEntityStatus(entity) == EntityStatus::NotAttached) {
		// Add Entity to the Disabled list. The Entity is not enabled by default.
		disabledEntities.emplace_back(entity);

		OnEntityAttach(entity);
		SetEntityStatus(entity, EntityStatus::Disabled);
	}
}

void System::DetachEntity(const Entity &entity) {
	const auto status = GetEntityStatus(entity);

	if (status != EntityStatus::NotAttached) {
		if (status == EntityStatus::Enabled) {
			// Remove Entity from Enabled list.
			enabledEntities.erase(std::remove(enabledEntities.begin(), enabledEntities.end(), entity), enabledEntities.end());
			OnEntityDisable(entity);
		} else {
			// Remove Entity from Disabled list.
			disabledEntities.erase(std::remove(disabledEntities.begin(), disabledEntities.end(), entity), disabledEntities.end());
		}

		OnEntityDetach(entity);
		SetEntityStatus(entity, EntityStatus::NotAttached);
	}
}

void System::EnableEntity(const Entity &entity) {
	if (GetEntityStatus(entity) == EntityStatus::Disabled) {
		// Remove Entity from Disabled list.
		disabledEntities.erase(std::remove(disabledEntities.begin(), disabledEntities.end(), entity), disabledEntities.end());

		// Then, add it to the Enabled list.
		enabledEntities.emplace_back(entity);

		OnEntityEnable(entity);
		SetEntityStatus(entity, EntityStatus::Enabled);
	}
}

void System::DisableEntity(const Entity &entity) {
	if (GetEntityStatus(entity) == EntityStatus::Enabled) {
		// Remove Entity from Enabled list.
		enabledEntities.erase(std::remove(enabledEntities.begin(), enabledEntities.end(), entity), enabledEntities.end());

		// Then, add it to the Disabled list.
		disabledEntities.emplace_back(entity);

		OnEntityDisable(entity);
		SetEntityStatus(entity, EntityStatus::Disabled);
	}
}

void System::OnStart() {
}

void System::OnShutdown() {
}

void System::OnEntityAttach(Entity entity) {
}

void System::OnEntityDetach(Entity entity) {
}

void System::OnEntityEnable(Entity entity) {
}

void System::OnEntityDisable(Entity entity) {
}

void System::Update(float delta) {
}

System::EntityStatus System::GetEntityStatus(Entity::Id id) const {
	const auto it = status.find(id);

	if (it != status.end()) {
		return it->second;
	}

	return EntityStatus::NotAttached;
}

void System::SetEntityStatus(Entity::Id id, const EntityStatus &status) {
	if (status == EntityStatus::NotAttached) {
		this->status.erase(id);
	} else {
		this->status[id] = status;
	}
}
}
