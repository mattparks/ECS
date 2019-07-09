#pragma once

#include <iostream>
#include <functional>
#include <memory>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>
#include "Helpers/NonCopyable.hpp"
#include "Helpers/TypeInfo.hpp"
#include "Scenes/System.hpp"

namespace ecs
{
class SystemHolder :
	public NonCopyable
{
public:
	SystemHolder() = default;

	~SystemHolder();

	/**
	 * Checks whether a System exists or not.
	 * @tparam T The System type.
	 * @return If the System exists.
	 */
	template<typename T>
	bool HasSystem() const
	{
		const auto it{m_systems.find(GetSystemTypeId<T>())};

		return it != m_systems.end() && it->second != nullptr;
	}

	/**
	 * Gets a System.
	 * @tparam T The System type.
	 * @return The System.
	 */
	template<typename T>
	T *GetSystem() const
	{
		auto it{m_systems.find(GetSystemTypeId<T>())};

		if (it == m_systems.end() || it->second == nullptr)
		{
			//throw std::runtime_error{"Scene does not have requested System"};
			return nullptr;
		}

		return static_cast<T *>(it->second.get());
	}

	/**
	 * Adds a System.
	 * @tparam T The System type.
	 * @param priority The System priority.
	 * @param system The System.
	 */
	template<typename T>
	void AddSystem(std::size_t priority, std::unique_ptr<T> &&system)
	{
		// Remove previous System, if it exists.
		RemoveSystem<T>();

		const auto typeId{GetSystemTypeId<T>()};

		// Insert the priority value
		m_priorities.insert({ priority, typeId });

		// Then, add the System
		m_systems[typeId] = std::move(system);
	}

	/**
	 * Removes a System.
	 * @tparam T The System type.
	 */
	template<typename T>
	void RemoveSystem()
	{
		const auto typeId{GetSystemTypeId<T>()};

		auto system{m_systems.find(typeId)};

		if (system != m_systems.end() && system->second != nullptr)
		{
			system->second->DetachAll();
		}

		// Remove the priority value for this System.
		RemoveSystemPriority(typeId);

		// Then, remove the System.
		m_systems.erase(typeId);
	}

	/**
	 * Removes all Systems.
	 */
	void RemoveAllSystems();

	/**
	 * Iterates through all valid Systems.
	 * @tparam Func The function type.
	 * @param func The function to pass each System into, System object and System ID.
	 */
	template<typename Func>
	void ForEach(Func &&func)
	{
		for (const auto &typeId : m_priorities)
		{
			auto &system{m_systems[typeId.second]};

			if (system != nullptr)
			{
				try
				{
					func(*system, typeId.second);
				}
				catch (const std::exception &e)
				{
					std::cerr << e.what() << '\n';
				}
			}
		}
	}

private:
	// Remove System from the priority list.
	void RemoveSystemPriority(const TypeId &id);

	// List of all Systems.
	std::unordered_map<TypeId, std::unique_ptr<System>> m_systems;

	// List of systems priorities.
	std::multimap<std::size_t, TypeId, std::greater<std::size_t>> m_priorities;
};
}
