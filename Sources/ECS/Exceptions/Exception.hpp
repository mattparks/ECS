#pragma once

#include <stdexcept>
#include <string>

namespace ecs
{
class Exception :
	public std::exception
{
public:
	Exception() = default;

	Exception(std::string const &message);

	Exception(std::string const &message, std::string const &prefix);

	~Exception() = default;

	// Exception message
	char const *what() const noexcept;

private:
	std::string m_message;
};
}
