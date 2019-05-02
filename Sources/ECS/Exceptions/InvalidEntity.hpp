#pragma once

#include "Exception.hpp"

namespace ecs
{
class InvalidEntity :
	public Exception
{
public:
	InvalidEntity(std::string const &function);

	~InvalidEntity() = default;
};
}
