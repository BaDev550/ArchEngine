#pragma once

#include <ArchEngine/Objects/Entity.h>
#include <ArchEngine/Core/Memory.h>
#include <ArchEngine/Scene/Scene.h>
#include <ArchEngine/Grapichs/Animator.h>
#include <ArchEngine/Grapichs/Animation.h>
#include <ArchEngine/Objects/Entity_Camera.h>
#include "PhysicsObject.h"

class Entity_Archer : public PhysicsObject {
public:
	REGISTER_ENTITY(Entity_Archer);

	Entity_Archer() {
		_type = ae::PhysicsMotionType::Dynamic;
		_boxColliderExtent = glm::vec3(0.2f, 0.5f, 0.25f);
		_boxColliderOffset = glm::vec3(0.0f, 0.6f, 0.0f);
	}

	virtual void OnCreate() override {
		_animator = ae::memory::Ref<ae::grapichs::Animator>::Create();
		SetRotation({ 0.0f, 0.0f, 0.0f });
		SetScale({ 0.01f, 0.01f, 0.01f });
		PhysicsObject::OnCreate();
		SetName("EntityArcher");
		RegisterAsDrawnable("Resources/Models/robot/ElyByKAtienza.dae");
		GetDrawnable().AnimatorInstance = _animator;
		_idleAnimation = GetDrawnable().ImportAnimation("Resources/Models/robot/Idle.dae");
		_walkingAnimation = GetDrawnable().ImportAnimation("Resources/Models/robot/Walking.dae");
		_danceAnimation = GetDrawnable().ImportAnimation("Resources/Models/robot/HipHopDancing.dae");
		_animator->PlayAnimation(_idleAnimation);
	};

	virtual void OnUpdate(float deltaTime) override {
		glm::vec3 velocity = GetVelocity();
		velocity.x = 0.0f;
		velocity.z = 0.0f;

		auto cameraEntities = GetScene()->Group<ae::Entity_Camera>();
		if (!cameraEntities.empty()) {
			ae::Entity_Camera* camEntity = dynamic_cast<ae::Entity_Camera*>(GetScene()->GetEntity(cameraEntities[0]));
			if (camEntity) {
				glm::vec3 camForward = camEntity->GetCamera()->GetForward();
				camForward.y = 0.0f;
				camForward = glm::normalize(camForward);

				glm::vec3 camRight = camEntity->GetCamera()->GetRight();
				camRight.y = 0.0f;
				camRight = glm::normalize(camRight);

				glm::vec3 moveDir(0.0f);
				if (ae::Input::IsKeyPressed(ae::key::W)) moveDir += camForward;
				if (ae::Input::IsKeyPressed(ae::key::S)) moveDir -= camForward;
				if (ae::Input::IsKeyPressed(ae::key::D)) moveDir += camRight;
				if (ae::Input::IsKeyPressed(ae::key::A)) moveDir -= camRight;

				if (glm::length(moveDir) > 0.0f) {
					moveDir = glm::normalize(moveDir);

					velocity.x = (moveDir.x * _speed) * deltaTime;
					velocity.z = (moveDir.z * _speed) * deltaTime;

					float targetAngle = atan2(moveDir.x, moveDir.z);
					SetRotation({ 0.0f, glm::degrees(targetAngle), 0.0f });
				}
			}
		}

		SetLinearVelocity(velocity);

		float horizontalSpeed = glm::length(glm::vec2(velocity.x, velocity.z));
		if (horizontalSpeed > 0.1f) {
			_animator->PlayAnimation(_walkingAnimation);
		}
		else {
			_animator->PlayAnimation(_idleAnimation);
		}

		ImGui::Begin("Archer");
		ImGui::Text("Speed: %f", GetVelocity().length());
		ImGui::Text("Horizontal Speed: %f", horizontalSpeed);
		ImGui::End();
	};

	virtual void OnDestroy() override {};
private:
	ae::memory::Ref<ae::grapichs::Animator> _animator;
	ae::grapichs::Animation* _idleAnimation = nullptr;
	ae::grapichs::Animation* _danceAnimation = nullptr;
	ae::grapichs::Animation* _walkingAnimation = nullptr;
	float _speed = 600.0f;
};