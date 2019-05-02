#include <iostream>

#include <ECS/Component.hpp>
#include <ECS/Entity.hpp>
#include <ECS/Event.hpp>
#include <ECS/EventDispatcher.hpp>
#include <ECS/Log.hpp>
#include <ECS/System.hpp>
#include <ECS/World.hpp>

#include <ECS/World.inl>
#include <ECS/Entity.inl>
#include <ECS/System.inl>

class Health :
	public ecs::Component
{
public:
	float healthPoints;
	float maxHealthPoints;
	float shield;
};

class HealthSystem :
	public ecs::System
{
public:
	HealthSystem()
	{
		GetFilter().Require<Health>();
	}

	void OnEntityAttached(ecs::Entity entity)
	{
		ecs::Log::Info("Entity created!");
	}
};

int main(int argc, char **argv)
{
	ecs::World world;

	world.AddSystem<HealthSystem>(0);

	if (world.HasSystem<HealthSystem>())
	{
		auto &healthSystem = world.GetSystem<HealthSystem>();
	}

	auto entity = world.CreateEntity();
	entity.AddComponent<Health>();

	if (entity.HasComponent<Health>())
	{
		auto &healthComponent = entity.GetComponent<Health>();
	}

	world.Update(1.0f / 60.0f);

	return 0;
}
