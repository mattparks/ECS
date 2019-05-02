#include "EventDispatcher.hpp"

namespace ecs
{
void EventDispatcher::ClearAll()
{
	m_listeners.clear();
}

void EventDispatcher::Clear(const Event::Id &id)
{
	for (auto it = m_listeners.begin(); it != m_listeners.end();)
	{
		if (it->second.id == id)
		{
			it = m_listeners.erase(it);
		}
		else
		{
			++it;
		}
	}
}
}
