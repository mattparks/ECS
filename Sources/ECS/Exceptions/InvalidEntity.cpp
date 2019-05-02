#include "InvalidEntity.hpp"

ecs::InvalidEntity::InvalidEntity(std::string const &function) :
	Exception("Invalid Entity.", function)
{
}
