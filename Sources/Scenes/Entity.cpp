#include "Entity.inl"

#include "World.hpp"
#include "Entity.hpp"

namespace ecs
{
Entity::Entity() :
	m_id(0)
{
}

Entity::Entity(const Id &id, World &world) :
	m_id(id),
	m_world(world)
{
}

void Entity::RemoveAllComponents()
{
	m_world.value()->m_components.RemoveAllComponents(m_id);
	m_world.value()->RefreshEntity(m_id);
}

std::string Entity::GetName() const
{
	return m_world.value()->GetEntityName(m_id);
}

bool Entity::IsEnabled() const
{
	return m_world.value()->IsEntityEnabled(m_id);
}

void Entity::Enable()
{
	m_world.value()->EnableEntity(m_id);
}

void Entity::Disable()
{
	m_world.value()->DisableEntity(m_id);
}

bool Entity::IsValid() const
{
	return m_world.has_value() && m_world.value()->IsEntityValid(m_id);
}

void Entity::Remove()
{
	m_world.value()->RemoveEntity(m_id);
}

bool Entity::operator==(const Entity &other) const
{
	return m_id == other.m_id && m_world == other.m_world;
}

bool Entity::operator!=(const Entity &other) const
{
	return !(*this == other);
}
}
