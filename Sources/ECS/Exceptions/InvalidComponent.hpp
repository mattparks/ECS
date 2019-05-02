#pragma once

#include "Exception.hpp"

namespace ecs
{
class InvalidComponent :
	public Exception
{
public:
	InvalidComponent(std::string const &function);

	~InvalidComponent() = default;
};
}
