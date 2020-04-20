#include "ComponentHolder.hpp"

namespace acid {
void ComponentHolder::RemoveAllComponents(Entity::Id id) {
	if (id < components.size()) {
		for (auto &component : components[id]) {
			component.reset();
		}

		componentsMasks[id].reset();
	}
}

ComponentFilter::Mask ComponentHolder::GetComponentsMask(Entity::Id id) const {
	if (id < componentsMasks.size()) {
		return componentsMasks[id];
	}

	return {};
}

void ComponentHolder::Resize(std::size_t size) {
	components.resize(size);
	componentsMasks.resize(size);
}

void ComponentHolder::Clear() noexcept {
	components.clear();
	componentsMasks.clear();
}
}
