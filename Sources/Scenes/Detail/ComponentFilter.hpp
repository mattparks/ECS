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

	ComponentFilter(ComponentFilter const &) noexcept = default;

	ComponentFilter(ComponentFilter &&) noexcept = default;

	ComponentFilter &operator=(ComponentFilter const &) noexcept = default;

	ComponentFilter &operator=(ComponentFilter &&) noexcept = default;

	// Check if an Entity matches the requirements
	bool Check(Mask const &mask) const;

	// Make a Component required
	template<class T>
	void Require()
	{
		m_required.set(GetComponentTypeId<T>());
		m_excluded.reset(GetComponentTypeId<T>());
	}

	// Make a Component excluded
	template<class T>
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
	template<class T>
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
