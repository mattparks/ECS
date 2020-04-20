#include <filesystem>

#include <Scenes/Component.hpp>
#include <Scenes/Entity.inl>
#include <Scenes/System.hpp>
#include <Scenes/Scene.hpp>

using namespace acid;

class Transform : public Component::Registrar<Transform> {
public:
	float x = 0.0f, y = 0.0f, z = 0.0f;
	float pitch = 0.0f, yaw = 0.0f, roll = 0.0f;
	float scale = 0.0f;

private:
	static inline bool registered = Register("transform");
};

class Collider {
public:
	const Transform &GetLocalTransform() const { return localTransform; }
	virtual void SetLocalTransform(const Transform &localTransform) = 0;

protected:
	Transform localTransform;
};

class ColliderSphere : public Collider {
public:
	explicit ColliderSphere(float radius) :
		radius(radius) {
	}

	void SetLocalTransform(const Transform &localTransform) override {
		std::cout << "Sphere r=" << radius << " set local transform value\n";
		this->localTransform = localTransform;
	}

	float radius;
};

class Rigidbody : public Component::Registrar<Rigidbody> {
public:
	Rigidbody() = default;
	explicit Rigidbody(std::unique_ptr<Collider> && collider) {
		colliders.emplace_back(std::move(collider));
	}
	explicit Rigidbody(std::vector<std::unique_ptr<Collider>> &&colliders) :
		colliders(std::move(colliders)) {
	}
	
	std::vector<std::unique_ptr<Collider>> colliders;

	static inline bool registered = Register("rigidbody");
};

class PhysicsSystem : public System {
public:
	PhysicsSystem() {
		GetFilter().Require<Transform>();
		GetFilter().Require<Rigidbody>();
	}

	void Update(float delta) override {
		ForEach([](Entity entity) {
			if (auto rigidbody = entity.GetComponent<Rigidbody>()) {
				rigidbody->colliders[0]->SetLocalTransform(*entity.GetComponent<Transform>());
			}
		});
		/*ForEach([](Entity entity, Transform *transform, Rigidbody *rigidbody) {
			rigidbody->colliders[0]->SetLocalTransform(*transform);
		});*/
	}
};

class Model {
public:
	explicit Model(std::filesystem::path filename) :
		filename(std::move(filename)) {
	}

	std::filesystem::path filename;
};

class Material {
public:
	explicit Material(float pipeline) :
		pipeline(pipeline) {
	}

	float pipeline;
};

class MaterialDefault : public Material {
public:
	MaterialDefault() :
		Material(-11.9f) {
	}
};

class MaterialSkybox : public Material {
public:
	MaterialSkybox() :
		Material(4.1f) {
	}
};

class Mesh : public Component::Registrar<Mesh> {
public:
	Mesh() = default;
	Mesh(std::unique_ptr<Model> &&model, std::unique_ptr<Material> &&material) :
		model(std::move(model)),
		material(std::move(material)) {
	}

	static inline bool registered = Register("mesh");
	
	std::unique_ptr<Model> model;
	std::unique_ptr<Material> material;
};

class MeshSystem : public System {
public:
	MeshSystem() {
		GetFilter().Require<Transform>();
		GetFilter().Require<Mesh>();
	}

	void OnStart() override {
		std::cout << "OnStart\n";
	}

	void OnShutdown() override {
		std::cout << "OnShutdown\n";
	}

	void OnEntityAttach(Entity entity) override {
		std::cout << "Entity attached: " << entity.GetComponent<Mesh>()->material->pipeline << '\n';
	}

	void OnEntityDetach(Entity entity) override {
		std::cout << "Entity detached: " << entity.GetComponent<Mesh>()->material->pipeline << '\n';
	}

	void OnEntityEnable(Entity entity) override {
		std::cout << "Entity enabled: " << entity.GetComponent<Mesh>()->material->pipeline << '\n';
	}

	void OnEntityDisable(Entity entity) override {
		std::cout << "Entity disabled: " << entity.GetComponent<Mesh>()->material->pipeline << '\n';
	}

	void Update(float delta) override {
		ForEach([](Entity entity) {
			std::cout << "Entity mesh update\n";
		});
	}
};

class TestScene : public Scene {
public:
	TestScene() : Scene(nullptr) {}
	void Start() override {}
	void Update() override {}
	bool IsPaused() const override { return false; }

};

int main(int argc, char **argv) {
	//auto materialDefault = Component::Create("materialDefault");
	//auto md = static_cast<MaterialDefault *>(materialDefault.get());

	std::unique_ptr<Scene> scene = std::make_unique<TestScene>();

	scene->AddSystem<PhysicsSystem>();
	scene->AddSystem<MeshSystem>();

	if (auto meshSystem = scene->GetSystem<MeshSystem>()) {
		std::cout << "Scene has mesh system\n";
	}

	auto entitySphere = scene->CreateEntity("sphere");

	auto entitySphereRef = *scene->GetEntity("sphere");
	entitySphereRef.AddComponent<Transform>();
	entitySphereRef.AddComponent<Rigidbody>(std::make_unique<ColliderSphere>(2.0f));
	entitySphereRef.AddComponent<Mesh>(std::make_unique<Model>("Sphere.obj"), std::make_unique<MaterialDefault>());
	std::cout << "Entity ref == entity: " << (entitySphereRef == entitySphere) << '\n';

	if (auto meshComponent = entitySphere.GetComponent<Mesh>()) {
		std::cout << "Entity has mesh\n";
	}

	auto entitySkybox = scene->CreateEntity();
	entitySkybox.AddComponent<Transform>();
	entitySkybox.AddComponent<Mesh>(std::make_unique<Model>("Cube.obj"), std::make_unique<MaterialSkybox>());

	scene->Update(1.0f / 60.0f);
	//entitySkybox.Remove();
	entitySkybox.RemoveComponent<Transform>();
	scene->Update(1.0f / 60.0f);

	// Pauses the console.
	std::cout << "Press enter to continue...";
	std::cin.get();
	return EXIT_SUCCESS;
}
