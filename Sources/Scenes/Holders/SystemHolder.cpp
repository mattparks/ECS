#include "SystemHolder.hpp"

namespace ecs
{
SystemHolder::~SystemHolder()
{
	RemoveAllSystems();
}

void SystemHolder::RemoveAllSystems()
{
	for (auto &system : m_systems)
	{
		if (system.second != nullptr)
		{
			system.second->DetachAll();
		}
	}

	m_systems.clear();
	m_priorities.clear();
}

void SystemHolder::RemoveSystemPriority(const TypeId &id)
{
	for (auto it = m_priorities.begin(); it != m_priorities.end();)
	{
		if (it->second == id)
		{
			it = m_priorities.erase(it);
		}
		else
		{
			++it;
		}
	}
}
}
