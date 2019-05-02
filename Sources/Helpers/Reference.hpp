#pragma once

#include <functional>
#include <memory>
#include <utility>

namespace ecs
{
template<typename T>
class Reference
{
public:
	Reference() noexcept :
		m_reference(nullptr)
	{
	}

	Reference(T &reference) noexcept :
		m_reference(std::addressof(reference))
	{
	}

	~Reference() = default;

	Reference(Reference const &) noexcept = default;

	Reference(Reference &&) noexcept = default;

	Reference &operator=(Reference const &) noexcept = default;

	Reference &operator=(Reference &&) noexcept = default;

	// Call the function of the stored reference
	T *operator->() const noexcept { return m_reference; }

	// Get the address of the stored reference
	T *operator&() const noexcept { return m_reference; }

	// Access the stored reference
	operator T &() const noexcept { return *m_reference; }

	// Access the stored reference
	T &get() const noexcept { return *m_reference; }

	// Call the stored function
	template<class... Args>
	std::invoke_result_t<T &, Args...> operator()(Args &&...args) const
	{
		return std::invoke(get(), std::forward<Args>(args)...);
	}

private:
	// Address of the referenced object
	T *m_reference;
};
}
