#include "Exception.hpp"

ecs::Exception::Exception(std::string const &message) :
	m_message(message)
{
}

ecs::Exception::Exception(std::string const &message, std::string const &prefix) :
	m_message(prefix + ": " + message)
{
}

char const *ecs::Exception::what() const noexcept
{
	return m_message.c_str();
}
