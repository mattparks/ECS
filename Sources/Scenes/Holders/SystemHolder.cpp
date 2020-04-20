#include "SystemHolder.hpp"

namespace acid {
SystemHolder::~SystemHolder() {
	RemoveAllSystems();
}

void SystemHolder::RemoveAllSystems() {
	for (auto &system : systems) {
		if (system.second) {
			system.second->OnShutdown();
			system.second->DetachAll();
		}
	}

	systems.clear();
	priorities.clear();
}

void SystemHolder::RemoveSystemPriority(TypeId id) {
	for (auto it = priorities.begin(); it != priorities.end();) {
		if (it->second == id) {
			it = priorities.erase(it);
		} else {
			++it;
		}
	}
}
}
