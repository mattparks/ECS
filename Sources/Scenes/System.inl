#pragma once

#include <type_traits>
#include <utility>
#include "EventDispatcher.hpp"
#include "Log.hpp"
#include "World.hpp"

template<class Func>
void ecs::System::ForEach(Func &&func)
{
	for (const auto &entity : m_enabledEntities)
	{
		if (entity.IsValid())
		{
			func(entity);
		}
	}
}

template<class T>
void ecs::System::EmitEvent(T const &evt) const
{
	GetWorld().m_evtDispatcher.Emit(evt);
}

template<class T, class Func>
ecs::Event::Id ecs::System::ConnectEvent(Func &&func)
{
	const auto id = GetWorld().m_evtDispatcher.Connect<T>(std::forward<Func>(func));

	// Save connection ID
	m_events.insert(id);

	return id;
}

template<class Func>
void ecs::System::CallEvent(Func &&func)
{
	try
	{
		func();
	}
	catch (std::exception const &e)
	{
		Log::Error(e.what());
	}
}

template<class T>
ecs::TypeId ecs::GetSystemTypeId() noexcept
{
	static_assert(std::is_base_of<System, T>::value, "T must be a System.");

	return TypeInfo<System>::GetTypeId<T>();
}
