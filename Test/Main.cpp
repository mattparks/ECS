#include <iostream>

#include <Scenes/Component.hpp>
#include <Scenes/Entity.hpp>
#include <Scenes/Event.hpp>
#include <Scenes/EventDispatcher.hpp>
#include <Scenes/Log.hpp>
#include <Scenes/System.hpp>
#include <Scenes/World.hpp>

class Material :
	public ecs::Component
{
public:
	float m_pipeline;
};

class MaterialSkybox :
	public Material
{
public:
};

class MaterialSystem :
	public ecs::System
{
public:
	MaterialSystem()
	{
		GetFilter().Require<Material>();
	}

	void OnEntityAttached(ecs::Entity entity)
	{
		ecs::Log::Warning("Entity created!");
	}
};

int main(int argc, char **argv)
{
	ecs::World world;

	world.AddSystem<MaterialSystem>(0);

	if (world.HasSystem<MaterialSystem>())
	{
		auto &materialSystem = world.GetSystem<MaterialSystem>();
	}

	auto entity = world.CreateEntity();
	entity.AddComponent<Material>();

	if (entity.HasComponent<Material>())
	{
		auto &materialComponent = entity.GetComponent<Material>();
	}

	world.Update(1.0f / 60.0f);

	return 0;
}
