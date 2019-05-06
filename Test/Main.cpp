#include <iostream>
#include <Engine/Log.hpp>
#include <Scenes/Component.hpp>
#include <Scenes/Entity.inl>
#include <Scenes/System.inl>
#include <Scenes/Scene.inl>

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

	void OnStart() override
	{
		Log::Out("OnStart\n");
	}

	void OnShutdown() override
	{
		Log::Out("OnShutdown\n");
	}

	void OnEntityAttach(Entity entity) override
	{
		Log::Out("Entity attached: %f\n", entity.GetComponent<MaterialSkybox>()->m_pipeline);
	}

	void OnEntityDetach(Entity entity) override
	{
		Log::Out("Entity detached: %f\n", entity.GetComponent<MaterialSkybox>()->m_pipeline);
	}

	void OnEntityEnable(Entity entity) override
	{
		Log::Out("Entity enabled: %f\n", entity.GetComponent<MaterialSkybox>()->m_pipeline);
	}

	void OnEntityDisable(Entity entity) override
	{
		Log::Out("Entity disabled: %f\n", entity.GetComponent<MaterialSkybox>()->m_pipeline);
	}

	void Update(const float &delta) override
	{
		ForEach([this](Entity entity)
		{
			Log::Out("Entity updated\n");
		});
	}
};

int main(int argc, char **argv)
{
	Scene scene;

	scene.AddSystem<MaterialSkyboxSystem>();

	if (scene.HasSystem<MaterialSkyboxSystem>())
	{
		auto materialSkyboxSystem = scene.GetSystem<MaterialSkyboxSystem>();
	}

	auto entitySphere = scene.CreateEntity("Sphere");

	auto entitySphereRef = *scene.GetEntity("Sphere");
	entitySphereRef.AddComponent<Transform>();
	entitySphereRef.AddComponent<MaterialDefault>();
	Log::Out("Entity ref == entity: %i\n", entitySphere == entitySphereRef);

	if (entitySphere.HasComponent<MaterialDefault>())
	{
		Log::Out("Entity has default material!\n");
		auto materialSkyboxComponent = entitySphere.GetComponent<MaterialDefault>();
	}

	auto entitySkybox = scene.CreateEntity();
	entitySkybox.AddComponent<Transform>();
	entitySkybox.AddComponent<MaterialSkybox>();

	scene.Update(1.0f / 60.0f);
	//entitySkybox.Remove();
	entitySkybox.RemoveComponent<Transform>();
	scene.Update(1.0f / 60.0f);

	// Pauses the console.
	std::cout << "Press enter to continue...";
	std::cin.get();
	return EXIT_SUCCESS;
}
