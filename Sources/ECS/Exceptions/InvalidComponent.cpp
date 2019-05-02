#include "InvalidComponent.hpp"

ecs::InvalidComponent::InvalidComponent(std::string const &function) :
	Exception("Invalid Component.", function)
{
}
