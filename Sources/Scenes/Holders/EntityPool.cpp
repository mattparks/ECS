#include "EntityPool.hpp"

namespace acid {
Entity::Id EntityPool::Create() {
	Entity::Id id;

	if (storedIds.empty()) {
		id = nextId;
		++nextId;
	} else {
		id = storedIds.back();
		storedIds.pop_back();
	}

	return id;
}

void EntityPool::Store(Entity::Id id) {
	if (id < nextId) {
		// Cannot store an ID that haven't been generated before.
		storedIds.emplace_back(id);
	}
}

void EntityPool::Reset() noexcept {
	storedIds.clear();
	nextId = 0;
}
}
