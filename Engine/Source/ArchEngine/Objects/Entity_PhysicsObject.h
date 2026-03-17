#pragma once

#include "Entity.h"
#include "ArchEngine/Scene/Scene.h"

namespace ae {
	class Entity_PhysicsObject : public Entity {
	public:
		REGISTER_ENTITY(Entity_PhysicsObject);

		Entity_PhysicsObject() {}

		virtual void OnCreate() override {
			SetName("Physics Object");
			RegisterAsPhysicsBody();
			GetPhysicsBody().MotionType = _type;
			GetPhysicsBody().CreateBoxCollider(_boxColliderExtent, _boxColliderOffset);
			GetPhysicsBody().CreateRigidBody();
		};

		void AddForce(const glm::vec3& force) { GetPhysicsBody().AddForce(force); }
		void SetLinearVelocity(const glm::vec3& velocity) { GetPhysicsBody().SetLinearVelocity(velocity); }
		void SetAngularVelocity(const glm::vec3& velocity) { GetPhysicsBody().SetAngularVelocity(velocity); }
		glm::vec3 GetVelocity() { return GetPhysicsBody().GetVelocity(); }

		virtual void OnUpdate(float deltaTime) override {};
		virtual void OnDestroy() override {};
	protected:
		glm::vec3 _boxColliderExtent = glm::vec3(1.0f);
		glm::vec3 _boxColliderOffset = glm::vec3(0.0f);
		ae::PhysicsMotionType _type = ae::PhysicsMotionType::Dynamic;
	};
}