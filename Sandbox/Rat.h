#pragma once

#include "PhysicsObject.h"

class Entity_Rat : public PhysicsObject {
public:
	Entity_Rat() {
		_type = ae::PhysicsMotionType::Dynamic;
		_boxColliderExtent = glm::vec3(0.3f, 0.1f, 0.3f);
	}

	virtual void OnCreate() override {
		SetPosition({ 0.0f, 10.0f, 0.0f });
		SetRotation({ -90.0f, 0.0f, 90.0f });
		SetScale({ 0.05f, 0.05f, 0.05f });
		PhysicsObject::OnCreate();
		SetName("Rat");
		RegisterAsDrawnable("Resources/Models/rat/scene.gltf");
	};

	virtual void OnUpdate(float deltaTime) override {};
	virtual void OnDestroy() override {};
};