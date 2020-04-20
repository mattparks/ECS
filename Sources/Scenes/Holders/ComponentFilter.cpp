#include "ComponentFilter.hpp"

namespace acid {
bool ComponentFilter::Check(const Mask &mask) const {
	const auto excludeMask = excluded & mask;

	// Checks if there is an excluded component.
	if (excludeMask.any()) {
		return false;
	}

	for (std::size_t i = 0; i < required.size(); ++i) {
		if (required[i] && !mask[i]) {
			// A required component is missing.
			return false;
		}
	}

	return true;
}

void ComponentFilter::ExcludeNotRequired() noexcept {
	excluded = ~required;
}

void ComponentFilter::ExcludeAll() noexcept {
	required.reset();
	excluded.set();
}
}
