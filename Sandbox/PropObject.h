#pragma once

#include "PhysicsObject.h"

class Entity_Prop : public PhysicsObject {
public:
	Entity_Prop(const std::string& path) : _propMeshPath(path) {
		_type = ae::PhysicsMotionType::Kinematic;
		_boxColliderExtent = glm::vec3(2.0f, 0.5f, 2.0f);
	}

	virtual void OnCreate() override {
		PhysicsObject::OnCreate();
		SetName("Prop Object");
		RegisterAsDrawnable(_propMeshPath);
	};

	virtual void OnUpdate(float deltaTime) override {};
	virtual void OnDestroy() override {};
private:
	std::string _propMeshPath;
};