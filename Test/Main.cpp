#include <iostream>
#include <Scenes/Component.hpp>
#include <Scenes/Entity.inl>
#include <Scenes/System.inl>
#include <Scenes/Scene.inl>

using namespace acid;

class Transform : public Component::Registrar<Transform> {
public:
	float m_x = 0.0f, m_y = 0.0f, m_z = 0.0f;
	float m_pitch = 0.0f, m_yaw = 0.0f, m_roll = 0.0f;
	float m_scale = 0.0f;

private:
	static bool registered;
};

bool Transform::registered = Register("transform");

class MaterialDefault : public Component::Registrar<MaterialDefault> {
public:
	float m_pipeline = -11.9f;

private:
	static bool registered;
};

bool MaterialDefault::registered = Register("materialDefault");

class MaterialSkybox : public Component::Registrar<MaterialSkybox> {
public:
	float m_pipeline = 4.1f;

private:
	static bool registered;
};

bool MaterialSkybox::registered = Register("materialSkybox");

class MaterialSkyboxSystem : public System {
public:
	MaterialSkyboxSystem() {
		GetFilter().Require<Transform>();
		//GetFilter().Require<Mesh | MeshAnimated>();
		GetFilter().Require<MaterialSkybox>();
	}

	void OnStart() override {
		std::cout << "OnStart\n";
	}

	void OnShutdown() override {
		std::cout << "OnShutdown\n";
	}

	void OnEntityAttach(Entity entity) override {
		std::cout << "Entity attached: " << entity.GetComponent<MaterialSkybox>()->m_pipeline << '\n';
	}

	void OnEntityDetach(Entity entity) override {
		std::cout << "Entity detached: " << entity.GetComponent<MaterialSkybox>()->m_pipeline << '\n';
	}

	void OnEntityEnable(Entity entity) override {
		std::cout << "Entity enabled: " << entity.GetComponent<MaterialSkybox>()->m_pipeline << '\n';
	}

	void OnEntityDisable(Entity entity) override {
		std::cout << "Entity disabled: " << entity.GetComponent<MaterialSkybox>()->m_pipeline << '\n';
	}

	void Update(float delta) override {
		ForEach([](Entity entity) {
			std::cout << "Entity updated\n";
		});
	}
};

int main(int argc, char **argv) {
	//auto materialDefault = Component::Create("MaterialDefault");
	//auto md = static_cast<MaterialDefault *>(materialDefault.get());

	Scene scene;

	scene.AddSystem<MaterialSkyboxSystem>();

	if (auto materialSkyboxSystem = scene.GetSystem<MaterialSkyboxSystem>()) {
	}

	auto entitySphere{scene.CreateEntity("Sphere")};

	auto entitySphereRef{*scene.GetEntity("Sphere")};
	entitySphereRef.AddComponent<Transform>();
	entitySphereRef.AddComponent<MaterialDefault>();
	std::cout << "Entity ref == entity: " << (entitySphereRef == entitySphere) << '\n';

	if (auto materialSkyboxComponent = entitySphere.GetComponent<MaterialDefault>()) {
		std::cout << "Entity has default material!\n";
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
