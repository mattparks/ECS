#include <iostream>
#include <Scenes/Component.hpp>
#include <Scenes/Entity.inl>
#include <Scenes/System.hpp>
#include <Scenes/Scene.hpp>

using namespace acid;

class Transform : public Component::Registrar<Transform> {
public:
	float m_x = 0.0f, m_y = 0.0f, m_z = 0.0f;
	float m_pitch = 0.0f, m_yaw = 0.0f, m_roll = 0.0f;
	float m_scale = 0.0f;

private:
	static inline bool registered = Register("transform");
};

class Collider {
public:
	const Transform &GetLocalTransform() const { return m_localTransform; }
	virtual void SetLocalTransform(const Transform &localTransform) = 0;

protected:
	Transform m_localTransform;
};

class ColliderSphere : public Collider {
public:
	explicit ColliderSphere(float radius) :
		m_radius(radius) {
	}

	void SetLocalTransform(const Transform &localTransform) override {
		std::cout << "Sphere r=" << m_radius << " set local transform value\n";
		m_localTransform = localTransform;
	}

	float m_radius;
};

class Rigidbody : public Component::Registrar<Rigidbody> {
public:
	Rigidbody() = default;
	explicit Rigidbody(std::unique_ptr<Collider> && collider) {
		m_colliders.emplace_back(std::move(collider));
	}
	explicit Rigidbody(std::vector<std::unique_ptr<Collider>> &&colliders) :
		m_colliders(std::move(colliders)) {
	}
	
	std::vector<std::unique_ptr<Collider>> m_colliders;

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
				rigidbody->m_colliders[0]->SetLocalTransform(*entity.GetComponent<Transform>());
			}
		});
		/*ForEach([](Entity entity, Transform *transform, Rigidbody *rigidbody) {
			rigidbody->m_colliders[0]->SetLocalTransform(*transform);
		});*/
	}
};

class Model {
public:
	explicit Model(std::filesystem::path filename) :
		m_filename(std::move(filename)) {
	}

	std::filesystem::path m_filename;
};

class Material {
public:
	explicit Material(float pipeline) :
		m_pipeline(pipeline) {
	}

	float m_pipeline;
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
		std::cout << "Entity attached: " << entity.GetComponent<Mesh>()->material->m_pipeline << '\n';
	}

	void OnEntityDetach(Entity entity) override {
		std::cout << "Entity detached: " << entity.GetComponent<Mesh>()->material->m_pipeline << '\n';
	}

	void OnEntityEnable(Entity entity) override {
		std::cout << "Entity enabled: " << entity.GetComponent<Mesh>()->material->m_pipeline << '\n';
	}

	void OnEntityDisable(Entity entity) override {
		std::cout << "Entity disabled: " << entity.GetComponent<Mesh>()->material->m_pipeline << '\n';
	}

	void Update(float delta) override {
		ForEach([](Entity entity) {
			std::cout << "Entity mesh update\n";
		});
	}
};

int main(int argc, char **argv) {
	//auto materialDefault = Component::Create("materialDefault");
	//auto md = static_cast<MaterialDefault *>(materialDefault.get());

	Scene scene;

	scene.AddSystem<PhysicsSystem>();
	scene.AddSystem<MeshSystem>();

	if (auto meshSystem = scene.GetSystem<MeshSystem>()) {
		std::cout << "Scene has mesh system\n";
	}

	auto entitySphere = scene.CreateEntity("sphere");

	auto entitySphereRef = *scene.GetEntity("sphere");
	entitySphereRef.AddComponent<Transform>();
	entitySphereRef.AddComponent<Rigidbody>(std::make_unique<ColliderSphere>(2.0f));
	entitySphereRef.AddComponent<Mesh>(std::make_unique<Model>("Sphere.obj"), std::make_unique<MaterialDefault>());
	std::cout << "Entity ref == entity: " << (entitySphereRef == entitySphere) << '\n';

	if (auto meshComponent = entitySphere.GetComponent<Mesh>()) {
		std::cout << "Entity has mesh\n";
	}

	auto entitySkybox = scene.CreateEntity();
	entitySkybox.AddComponent<Transform>();
	entitySkybox.AddComponent<Mesh>(std::make_unique<Model>("Cube.obj"), std::make_unique<MaterialSkybox>());

	scene.Update(1.0f / 60.0f);
	//entitySkybox.Remove();
	entitySkybox.RemoveComponent<Transform>();
	scene.Update(1.0f / 60.0f);

	// Pauses the console.
	std::cout << "Press enter to continue...";
	std::cin.get();
	return EXIT_SUCCESS;
}
