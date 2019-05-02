#pragma once

#include <type_traits>
#include <utility>
#include "EventDispatcher.hpp"
#include "Log.hpp"
#include "World.hpp"

namespace ecs
{
template<class Func>
void System::ForEach(Func &&func)
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
void System::EmitEvent(T const &evt) const
{
	GetWorld().m_evtDispatcher.Emit(evt);
}

template<class T, class Func>
Event::Id System::ConnectEvent(Func &&func)
{
	const auto id = GetWorld().m_evtDispatcher.Connect<T>(std::forward<Func>(func));

	// Save connection ID
	m_events.insert(id);

	return id;
}

template<class Func>
void System::CallEvent(Func &&func)
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
TypeId GetSystemTypeId() noexcept
{
	static_assert(std::is_base_of<System, T>::value, "T must be a System.");

	return TypeInfo<System>::GetTypeId<T>();
}
}
