#pragma once

#include <type_traits>
#include <utility>
#include "Engine/Log.hpp"
#include "EventDispatcher.hpp"
#include "World.hpp"

namespace ecs
{
template<typename Func>
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

template<typename T>
void System::EmitEvent(const T &evt) const
{
	GetWorld().m_evtDispatcher.Emit(evt);
}

template<typename T, typename Func>
Event::Id System::ConnectEvent(Func &&func)
{
	const auto id = GetWorld().m_evtDispatcher.Connect<T>(std::forward<Func>(func));

	// Save connection ID
	m_events.insert(id);

	return id;
}

template<typename Func>
void System::CallEvent(Func &&func)
{
	try
	{
		func();
	}
	catch (const std::exception &e)
	{
		Log::Error(e.what());
	}
}

template<typename T>
TypeId GetSystemTypeId() noexcept
{
	static_assert(std::is_base_of<System, T>::value, "T must be a System.");

	return TypeInfo<System>::GetTypeId<T>();
}
}
