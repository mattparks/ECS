#include "ComponentFilter.hpp"

namespace ecs
{
	void ComponentFilter::ExcludeAll() noexcept
	{
		m_required.reset();
		m_excluded.set();
	}

	void ComponentFilter::ExcludeNotRequired() noexcept
	{
		m_excluded = ~m_required;
	}

	bool ComponentFilter::Check(Mask const& mask) const
	{
		const auto excludeMask = m_excluded & mask;

		// Check if there is an excluded component
		if (excludeMask.any())
		{
			return false;
		}

		for (std::size_t i = 0; i < m_required.size(); ++i)
		{
			if (m_required[i] && !mask[i])
			{
				// A required component is missing
				return false;
			}
		}

		return true;
	}
}
