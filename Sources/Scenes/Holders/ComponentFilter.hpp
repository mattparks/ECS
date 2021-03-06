#pragma once

#include <bitset>
#include "Scenes/Component.hpp"

namespace acid {
class ACID_EXPORT ComponentFilter {
public:
	using Mask = std::bitset<MAX_COMPONENTS>;

	ComponentFilter() = default;
	~ComponentFilter() = default;

	/**
	 * Checks if an Entity matches the requirements.
	 * @param mask The requirements mask.
	 * @return If the Entity matches.
	 */
	bool Check(const Mask &mask) const;

	/**
	 * Makes a Component required.
	 * @tparam T The Component type.
	 */
	template<typename T>
	void Require() {
		required.set(GetComponentTypeId<T>());
		excluded.reset(GetComponentTypeId<T>());
	}

	/**
	 * Makes a Component excluded.
	 * @tparam T The Component type.
	 */
	template<typename T>
	void Exclude() {
		required.reset(GetComponentTypeId<T>());
		excluded.set(GetComponentTypeId<T>());
	}

	/**
	 * Exclude all Components that are not required.
	 */
	void ExcludeNotRequired() noexcept;

	/**
	 * Exclude all Components.
	 */
	void ExcludeAll() noexcept;

	/**
	 * Removes a Component from both required and excluded lists.
	 * @tparam T The Component type.
	 */
	template<typename T>
	void Ignore() {
		required.reset(GetComponentTypeId<T>());
		excluded.reset(GetComponentTypeId<T>());
	}

private:
	Mask required;
	Mask excluded;
};
}
