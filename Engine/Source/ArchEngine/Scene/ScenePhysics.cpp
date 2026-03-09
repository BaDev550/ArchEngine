#include "ArchPch.h"
#include "ScenePhysics.h"

#include "ArchEngine/Physics/PhysicsEngine.h"
#include "ArchEngine/Scene/Scene.h"
#include "ArchEngine/Utilities/Math.h"

namespace ae {
	ScenePhysics::ScenePhysics(Scene* scene) : _scene(scene) {}
	ScenePhysics::~ScenePhysics() {}

	PhysicsHandle ScenePhysics::CreatePhysicsBody(EntityID owner)
	{
		_physicsBodies.emplace_back(owner, this);
		return PhysicsHandle{ _physicsBodies.size() - 1 };
	}

	PhysicsBody& ScenePhysics::GetPhysicsBody(const PhysicsHandle& handle)
	{
		if (handle.IsValid() && handle.index < _physicsBodies.size()) {
			return _physicsBodies[handle.index];
		}
		throw std::runtime_error("Invalid PhysicsHandle");
	}

	void ScenePhysics::RemoveAllPhysicsBody() {
		_physicsBodies.clear();
	}

	void ScenePhysics::Step(float deltaTime, std::unordered_map<EntityID, memory::Ref<Entity>>& entities)
	{
		auto& bodyInterface = physics::PhysicsEngine::GetBodyInterface();
		for (auto& body : _physicsBodies) {
			if (!body.IsValid())
				continue;

			auto entityIt = entities.find(body.OwnerID);
			if (entityIt == entities.end())
				continue;

			auto& entity = entityIt->second;
			if (body.MotionType == PhysicsMotionType::Static) {
				bodyInterface.SetPositionAndRotation(
					body.JoltBodyID, 
					math::GlmToJolt(entity->GetPosition()), 
					math::GlmToJolt(entity->GetRotation()),
					JPH::EActivation::Activate
				);
			}
		}

		physics::PhysicsEngine::Update(deltaTime);

		for (auto& body : _physicsBodies) {
			if (!body.IsValid())
				continue;

			if (body.MotionType != PhysicsMotionType::Dynamic)
				continue;

			auto entityIt = entities.find(body.OwnerID);
			if (entityIt != entities.end()) {
				auto& entity = entityIt->second;
				entity->SetPosition(math::JoltToGlm(body.GetRigidBody()->GetPosition()));
				entity->SetRotation(math::JoltToGlm(body.GetRigidBody()->GetRotation()));
			}
		}
	}

	PhysicsBody::~PhysicsBody()
	{
		JPH::BodyInterface& interface = physics::PhysicsEngine::GetBodyInterface();
		if (IsValid()) {
			interface.RemoveBody(JoltBodyID);
			interface.DestroyBody(JoltBodyID);
			_rgBody = nullptr;
		}
	}

	void PhysicsBody::CreateBoxCollider(const glm::vec3& boxExtent, const glm::vec3& offset)
	{
		JPH::BoxShapeSettings boxSettings(math::GlmToJolt(boxExtent));
		JPH::Shape::ShapeResult boxResult = boxSettings.Create();

		if (boxResult.HasError()) {
			Logger_app::error("Failed to create box shape: {}", boxResult.GetError().c_str());
			return;
		}
		JPH::RotatedTranslatedShapeSettings settings(math::GlmToJolt(offset), JPH::Quat::sIdentity(), boxResult.Get());
		JPH::Shape::ShapeResult finalResult = settings.Create();
		if (!finalResult.HasError()) {
			_shape = finalResult.Get();
		}
		_shapeOffset = offset;
	}

	void PhysicsBody::CreateSphereCollider()
	{
	}

	void PhysicsBody::CreateRigidBody()
	{
		if (!_shape)
			return;

		auto& ownerEntity = _ownerSystem->_scene->GetEntity(OwnerID);
		JPH::BodyInterface& interface = physics::PhysicsEngine::GetBodyInterface();
		JPH::BodyCreationSettings settings(
			_shape,
			math::GlmToJolt(ownerEntity.GetPosition()),
			math::GlmToJolt(ownerEntity.GetRotation()),
			PhysicsMotionTypeToJoltType(),
			physics::Layers::MOVING
		);
		_rgBody = interface.CreateBody(settings);
		if (!_rgBody) {
			Logger_app::error("Failed to create rigidbody");
			return;
		}

		JoltBodyID = _rgBody->GetID();
		interface.AddBody(JoltBodyID, JPH::EActivation::Activate);
	}

	void PhysicsBody::AddForce(const glm::vec3& force) {
		JPH::BodyInterface& interface = physics::PhysicsEngine::GetBodyInterface();
		interface.AddForce(JoltBodyID, math::GlmToJolt(force));
	}

	void PhysicsBody::SetLinearVelocity(const glm::vec3& velocity) {
		JPH::BodyInterface& interface = physics::PhysicsEngine::GetBodyInterface();
		interface.SetLinearVelocity(JoltBodyID, math::GlmToJolt(velocity));
	}

	void PhysicsBody::SetAngularVelocity(const glm::vec3& velocity) {
		JPH::BodyInterface& interface = physics::PhysicsEngine::GetBodyInterface();
		interface.SetAngularVelocity(JoltBodyID, math::GlmToJolt(velocity));
	}

	glm::vec3 PhysicsBody::GetVelocity() const {
		return math::JoltToGLM(_rgBody->GetLinearVelocity());
	}
}