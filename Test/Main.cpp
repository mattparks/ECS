#include <iostream>
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
	float m_pipeline{-11.9f};
};

class MaterialSkybox :
	public Component
{
public:
	float m_pipeline{4.1f};
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
		std::cout << "OnStart\n";
	}

	void OnShutdown() override
	{
		std::cout << "OnShutdown\n";
	}

	void OnEntityAttach(Entity entity) override
	{
		std::cout << "Entity attached: " << entity.GetComponent<MaterialSkybox>()->m_pipeline << '\n';
	}

	void OnEntityDetach(Entity entity) override
	{
		std::cout << "Entity detached: " << entity.GetComponent<MaterialSkybox>()->m_pipeline << '\n';
	}

	void OnEntityEnable(Entity entity) override
	{
		std::cout << "Entity enabled: " << entity.GetComponent<MaterialSkybox>()->m_pipeline << '\n';
	}

	void OnEntityDisable(Entity entity) override
	{
		std::cout << "Entity disabled: " << entity.GetComponent<MaterialSkybox>()->m_pipeline << '\n';
	}

	void Update(const float &delta) override
	{
		ForEach([](Entity entity)
		{
			std::cout << "Entity updated\n";
		});
	}
};

int main(int argc, char **argv)
{
	Scene scene;

	scene.AddSystem<MaterialSkyboxSystem>();

	if (auto materialSkyboxSystem{scene.GetSystem<MaterialSkyboxSystem>()}; materialSkyboxSystem != nullptr)
	{
	}

	auto entitySphere{scene.CreateEntity("Sphere")};

	auto entitySphereRef{*scene.GetEntity("Sphere")};
	entitySphereRef.AddComponent<Transform>();
	entitySphereRef.AddComponent<MaterialDefault>();
	std::cout << "Entity ref == entity: ", entitySphere == entitySphereRef << '\n';

	if (auto materialSkyboxComponent{entitySphere.GetComponent<MaterialDefault>()}; materialSkyboxComponent != nullptr)
	{
		std::cout << "Entity has default material!\n";
	}

	auto entitySkybox{scene.CreateEntity()};
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
