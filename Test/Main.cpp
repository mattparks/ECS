#include <iostream>

#include <Scenes/Component.hpp>
#include <Scenes/Entity.hpp>
#include <Scenes/Event.hpp>
#include <Scenes/EventDispatcher.hpp>
#include <Scenes/Log.hpp>
#include <Scenes/System.hpp>
#include <Scenes/World.hpp>
		  
#include <Scenes/World.inl>
#include <Scenes/Entity.inl>
#include <Scenes/System.inl>

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
