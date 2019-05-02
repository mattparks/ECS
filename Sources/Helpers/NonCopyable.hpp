#pragma once

namespace ecs
{
/**
 * @brief Class that makes derived classes non-copyable.
 */
class NonCopyable
{
protected:
	NonCopyable() = default;

	virtual ~NonCopyable() = default;

public:
	NonCopyable(const NonCopyable &) = delete;

	NonCopyable &operator=(const NonCopyable &) = delete;
};
}
