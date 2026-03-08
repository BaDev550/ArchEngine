#pragma once

#include <ArchEngine/Objects/Entity.h>
#include <ArchEngine/Scene/Scene.h>

class PhysicsObject : public ae::Entity {
public:
	REGISTER_ENTITY(PhysicsObject);

	PhysicsObject() {}

	virtual void OnCreate() override {
		SetName("Physics Object");
		RegisterAsPhysicsBody();
		GetPhysicsBody().MotionType = _type;
		GetPhysicsBody().CreateBoxCollider(_boxColliderExtent);
		GetPhysicsBody().CreateRigidBody();
	};

	virtual void OnUpdate(float deltaTime) override {};
	virtual void OnDestroy() override {};
protected:
	glm::vec3 _boxColliderExtent = { 1.0f, 1.0f, 1.0f };
	ae::PhysicsMotionType _type = ae::PhysicsMotionType::Dynamic;
};