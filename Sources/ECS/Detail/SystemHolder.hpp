#pragma once

#include <functional>
#include <memory>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>
#include "Helpers/NonCopyable.hpp"
#include "ECS/Exceptions/Exception.hpp"
#include "ECS/Log.hpp"
#include "ECS/System.hpp"
#include "TypeInfo.hpp"

namespace ecs::detail
{
class SystemHolder :
	public NonCopyable
{
public:
	SystemHolder() = default;

	~SystemHolder();

	SystemHolder(SystemHolder &&) = default;

	SystemHolder &operator=(SystemHolder &&) = default;

	// Add a System
	template<class T>
	void AddSystem(std::size_t priority, std::unique_ptr<T> &&system)
	{
		// Remove previous System, if exists
		RemoveSystem<T>();

		const auto typeId = GetSystemTypeId<T>();

		// Insert the priority value
		m_priorities.insert({ priority, typeId });

		// Then, add the System
		m_systems[typeId] = std::move(system);
	}

	// Get a System
	template<class T>
	T &GetSystem()
	{
		auto it = m_systems.find(GetSystemTypeId<T>());

		if (it == m_systems.end() || it->second == nullptr)
		{
			throw Exception("World does not have this System.", "ecs::World::GetSystem()");
		}

		return *static_cast<T*>(it->second.get());
	}

	// Get a System
	template<class T>
	T const &GetSystem() const
	{
		auto it = m_systems.find(GetSystemTypeId<T>());

		if (it == m_systems.end() || it->second == nullptr)
		{
			throw Exception("World does not have this System.", "ecs::World::GetSystem()");
		}

		return *static_cast<T*>(it->second.get());
	}

	// Check whether a System exists or not
	template<class T>
	bool HasSystem() const
	{
		const auto it = m_systems.find(GetSystemTypeId<T>());

		return it != m_systems.end() && it->second != nullptr;
	}

	// Remove a System
	template<class T>
	void RemoveSystem()
	{
		const auto typeId = GetSystemTypeId<T>();

		auto system = m_systems.find(typeId);

		if (system != m_systems.end() && system->second != nullptr)
		{
			system->second->OnShutdown();
			system->second->DetachAll();
		}

		// Remove the priority value for this System
		RemoveSystemPriority(typeId);

		// Then, remove the System
		m_systems.erase(typeId);
	}

	// Remove all Systems
	void RemoveAllSystems();

	// Iterate through all valid Systems
	template<class Func>
	void ForEach(Func &&func)
	{
		for (const auto& typeId : m_priorities)
		{
			auto& system = m_systems[typeId.second];

			if (system != nullptr)
			{
				try
				{
					func(*system, typeId.second);
				}
				catch (std::exception const& e)
				{
					Log::Error(e.what());
				}
			}
		}
	}

private:
	// Remove System from the priority list
	void RemoveSystemPriority(detail::TypeId id);

	// List of all Systems
	std::unordered_map<detail::TypeId, std::unique_ptr<System>> m_systems;

	// List of systems priorities
	std::multimap<std::size_t, detail::TypeId, std::greater<std::size_t>> m_priorities;
};
}
