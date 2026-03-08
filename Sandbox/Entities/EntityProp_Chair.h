#pragma once

#include "PhysicsObject.h"

class EntityProp_Chair : public PhysicsObject {
public:
	REGISTER_ENTITY(EntityProp_Chair);

	EntityProp_Chair() : _propMeshPath("Resources/Models/chair/scene.gltf") {
		_type = ae::PhysicsMotionType::Kinematic;
		_boxColliderExtent = glm::vec3(2.0f, 0.5f, 2.0f);
	}

	virtual void OnCreate() override {
		PhysicsObject::OnCreate();
		SetName("Chair Object");
		RegisterAsDrawnable(_propMeshPath);
	};

	virtual void OnUpdate(float deltaTime) override {};
	virtual void OnDestroy() override {};
private:
	std::string _propMeshPath;
};