#pragma once

#include "PhysicsObject.h"

class Entity_Box : public PhysicsObject {
public:
	REGISTER_ENTITY(Entity_Box);

	Entity_Box() {
		_type = ae::PhysicsMotionType::Kinematic;
		_boxColliderExtent = glm::vec3(15.0f, 0.2f, 15.0f);
	}

	virtual void OnCreate() override {
		SetPosition({ 0.0f, -0.5f, 0.0f });
		SetScale({ 15.0f, 0.2f, 15.0f });
		PhysicsObject::OnCreate();
		SetName("Box Object");
		RegisterAsDrawnable("Resources/Models/CheckerBox/Cube.obj");
	};

	virtual void OnUpdate(float deltaTime) override {};
	virtual void OnDestroy() override {};
};