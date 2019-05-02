#pragma once

#include <bitset>
#include "Scenes/Component.hpp"

namespace ecs
{
class ComponentFilter
{
public:
	using Mask = std::bitset<MAX_COMPONENTS>;

	ComponentFilter() noexcept = default;

	~ComponentFilter() = default;

	ComponentFilter(const ComponentFilter &) noexcept = default;

	ComponentFilter(ComponentFilter &&) noexcept = default;

	ComponentFilter &operator=(const ComponentFilter &) noexcept = default;

	ComponentFilter &operator=(ComponentFilter &&) noexcept = default;

	// Check if an Entity matches the requirements
	bool Check(const Mask &mask) const;

	// Make a Component required
	template<typename T>
	void Require()
	{
		m_required.set(GetComponentTypeId<T>());
		m_excluded.reset(GetComponentTypeId<T>());
	}

	// Make a Component excluded
	template<typename T>
	void Exclude()
	{
		m_required.reset(GetComponentTypeId<T>());
		m_excluded.set(GetComponentTypeId<T>());
	}

	// Exclude all Components that are not required
	void ExcludeNotRequired() noexcept;

	// Exclude all Components
	void ExcludeAll() noexcept;

	// Remove a Component from both lists
	template<typename T>
	void Ignore()
	{
		m_required.reset(GetComponentTypeId<T>());
		m_excluded.reset(GetComponentTypeId<T>());
	}

private:
	Mask m_required;
	Mask m_excluded;
};
}
