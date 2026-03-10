#pragma once

#include "PhysicsObject.h"

class Entity_Mario : public PhysicsObject {
public:
	REGISTER_ENTITY(Entity_Mario);

	Entity_Mario() {
		_type = ae::PhysicsMotionType::Kinematic;
		_boxColliderExtent = glm::vec3(1.0f, 1.5f, 1.0f);
	}

	virtual void OnCreate() override {
		PhysicsObject::OnCreate();
		SetName("Mario Object");
		RegisterAsDrawnable("Resources/Models/mario_2/mario_2.obj");
	};

	virtual void OnUpdate(float deltaTime) override {};
	virtual void OnDestroy() override {};
};