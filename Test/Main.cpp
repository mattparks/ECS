#include <iostream>

#include <Engine/Log.hpp>
#include <Scenes/Component.hpp>
#include <Scenes/Entity.inl>
#include <Scenes/Event.hpp>
#include <Scenes/EventDispatcher.hpp>
#include <Scenes/System.inl>
#include <Scenes/World.inl>

class Transform :
	public ecs::Component
{
public:
	float m_x, m_y, m_z;
	float m_pitch, m_yaw, m_roll;
	float m_scale;
};

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
		GetFilter().Require<Transform>();
		GetFilter().Require<Material>();
	}

	void OnEntityAttached(ecs::Entity entity)
	{
		ecs::Log::Out("Entity created!");
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
	entity.AddComponent<Transform>();
	entity.AddComponent<Material>();

	if (entity.HasComponent<Material>())
	{
		auto &materialComponent = entity.GetComponent<Material>();
		materialComponent.m_pipeline = 10.0f;
	}

	world.Update(1.0f / 60.0f);

	return 0;
}
