#include <iostream>

#include <Engine/Log.hpp>
#include <Scenes/Component.hpp>
#include <Scenes/Entity.inl>
#include <Scenes/Event.hpp>
#include <Scenes/EventDispatcher.hpp>
#include <Scenes/System.inl>
#include <Scenes/World.inl>

using namespace ecs;

class Transform :
	public Component
{
public:
	float m_x, m_y, m_z;
	float m_pitch, m_yaw, m_roll;
	float m_scale;
};

class MaterialDefault :
	public Component
{
public:
	float m_pipeline = -11.9f;
};

class MaterialSkybox :
	public Component
{
public:
	float m_pipeline = 4.1f;
};

class MaterialSkyboxSystem :
	public System
{
public:
	MaterialSkyboxSystem()
	{
		GetFilter().Require<Transform>();
		//GetFilter().Require<Mesh | MeshAnimated>();
		GetFilter().Require<MaterialSkybox>();
	}

	void OnEntityAttached(Entity entity)
	{
		Log::Out("Entity with skybox material created: %f\n", entity.GetComponent<MaterialSkybox>().m_pipeline);
	}
};

int main(int argc, char **argv)
{
	World world;

	world.AddSystem<MaterialSkyboxSystem>(0);

	if (world.HasSystem<MaterialSkyboxSystem>())
	{
		auto &materialSkyboxSystem = world.GetSystem<MaterialSkyboxSystem>();
	}

	auto entitySphere = world.CreateEntity("Sphere");

	auto entitySphereRef = *world.GetEntity("Sphere");
	entitySphereRef.AddComponent<Transform>();
	entitySphereRef.AddComponent<MaterialDefault>();
	Log::Out("Entity ref == entity: %i\n", entitySphere == entitySphereRef);

	if (entitySphere.HasComponent<MaterialDefault>())
	{
		Log::Out("Entity has default material!\n");
		auto &materialSkyboxComponent = entitySphere.GetComponent<MaterialDefault>();
	}

	auto entitySkybox = world.CreateEntity();
	entitySkybox.AddComponent<Transform>();
	entitySkybox.AddComponent<MaterialSkybox>();

	world.Update(1.0f / 60.0f);

	return 0;
}
