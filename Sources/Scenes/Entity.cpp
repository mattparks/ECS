#include "Entity.inl"

#include "World.hpp"

namespace ecs
{
Entity::Entity(Id id, World &world) :
	m_id(id),
	m_world(world)
{
}

Entity::operator Id() const noexcept
{
	return m_id;
}

Entity::Id Entity::GetId() const noexcept
{
	return m_id;
}

void Entity::RemoveAllComponents()
{
	m_world.value()->m_components.RemoveAllComponents(m_id);
	m_world.value()->RefreshEntity(m_id);
}

void Entity::Enable()
{
	m_world.value()->EnableEntity(m_id);
}

void Entity::Disable()
{
	m_world.value()->DisableEntity(m_id);
}

bool Entity::IsEnabled() const
{
	return m_world.value()->IsEntityEnabled(m_id);
}

bool Entity::IsValid() const
{
	return m_world.has_value() && m_world.value()->IsEntityValid(m_id);
}

std::string Entity::GetName() const
{
	return m_world.value()->GetEntityName(m_id);
}

void Entity::Remove()
{
	m_world.value()->RemoveEntity(m_id);
}

bool Entity::operator==(Entity const &rhs) const
{
	return m_id == rhs.m_id && &m_world.value() == &rhs.m_world.value();
}

bool Entity::operator!=(Entity const &rhs) const
{
	return !operator==(rhs);
}

std::size_t Entity::Hash::operator()(Entity const &entity) const
{
	return std::hash<Entity::Id>()(entity.m_id);
}
}
