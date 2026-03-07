#pragma once

#include <ArchEngine/Objects/Entity.h>
#include <ArchEngine/Scene/Scene.h>

class PhysicsObject : public ae::Entity {
public:
	PhysicsObject(ae::PhysicsMotionType type, const glm::vec3& pos) : _type(type) {
		SetPosition(pos);
	}

	virtual void OnCreate() override {
		SetName("Physics Object");
		RegisterAsDrawnable("Resources/Models/mario_2/mario_2.obj");
		RegisterAsPhysicsBody();
		GetPhysicsBody().MotionType = _type;
		GetPhysicsBody().CreateBoxCollider({1.0f, 1.0f, 1.0f});
		GetPhysicsBody().CreateRigidBody();
	};

	virtual void OnUpdate(float deltaTime) override {};
	virtual void OnDestroy() override {};
private:
	ae::PhysicsMotionType _type = ae::PhysicsMotionType::Dynamic;
};