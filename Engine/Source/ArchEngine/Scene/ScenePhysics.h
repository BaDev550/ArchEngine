#pragma once
#include "ArchEngine/Objects/Entity.h"
#include "ArchEngine/Core/Memory.h"

#include <glm/glm.hpp>
#include <vector>
#include <deque>
#include <unordered_map>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/CollideShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/OffsetCenterOfMassShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>

namespace ae {
#define INVALID_JOLT_BODY_ID 0xFFFFFFFF;
	enum class PhysicsMotionType {
		Static = 0,
		Kinematic,
		Dynamic
	};

	class ScenePhysics;
	struct PhysicsBody {
		PhysicsBody(EntityID owner, ScenePhysics* ownerSystem) : OwnerID(owner), _ownerSystem(ownerSystem) {}
		~PhysicsBody();
		PhysicsBody(const PhysicsBody&) = delete;
		PhysicsBody& operator=(const PhysicsBody&) = delete;

		EntityID OwnerID = 0;
		JPH::BodyID JoltBodyID;
		PhysicsMotionType MotionType = PhysicsMotionType::Static;

		JPH::Body* GetRigidBody() { return _rgBody; }
		bool IsValid() const { return !JoltBodyID.IsInvalid(); }
		void CreateBoxCollider(const glm::vec3& boxExtent, const glm::vec3& offset = glm::vec3(0.0f));
		void CreateSphereCollider();
		void CreateRigidBody();

		void AddForce(const glm::vec3& force);
		void SetLinearVelocity(const glm::vec3& velocity);
		void SetAngularVelocity(const glm::vec3& velocity);
		glm::vec3 GetVelocity() const;
		glm::vec3 GetOffset() const { return _shapeOffset; }
	private:
		JPH::Body* _rgBody;
		JPH::Ref<JPH::Shape> _shape;
		glm::vec3 _shapeOffset = glm::vec3(0.0f);
		JPH::EMotionType PhysicsMotionTypeToJoltType() {
			switch (MotionType)
			{
			case ae::PhysicsMotionType::Static: return JPH::EMotionType::Static;
			case ae::PhysicsMotionType::Kinematic: return JPH::EMotionType::Kinematic;
			case ae::PhysicsMotionType::Dynamic: return JPH::EMotionType::Dynamic;
			default:
				Logger_app::error("INVALID MOTION TYPE");
				break;
			}
		}
		ScenePhysics* _ownerSystem;
	};

	class ScenePhysics {
	public:
		ScenePhysics(Scene* scene);
		~ScenePhysics();
		PhysicsHandle CreatePhysicsBody(EntityID owner);
		PhysicsBody& GetPhysicsBody(const PhysicsHandle& handle);
		void RemoveAllPhysicsBody();
		void Step(float deltaTime, std::unordered_map<EntityID, memory::Ref<Entity>>& entities);
	private:
		Scene* _scene = nullptr;
		std::deque<PhysicsBody> _physicsBodies;
		friend class PhysicsBody;
	};
}