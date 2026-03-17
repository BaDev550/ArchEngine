#pragma once

#include <ArchEngine/Objects/Entity.h>
#include <ArchEngine/Objects/Entity_PhysicsObject.h>
#include <ArchEngine/Grapichs/Animator.h>
#include "Common/Defines.h"

using namespace ae;
class Entity_Player : public Entity_PhysicsObject {
public:
	REGISTER_ENTITY(Entity_Player);

	Entity_Player() {
		_animator = memory::Ref<grapichs::Animator>::Create();
		_type = PhysicsMotionType::Dynamic;
		_boxColliderExtent = { 0.5f, 1.0f, 0.5f };
		_boxColliderOffset = { 0.0f, 1.0f, 0.0f };
		SetScale({ 0.01f, 0.01f, 0.01f });
		SetPosition({ 0.0f, 10.0f, 0.0f });
	}

	virtual void OnCreate() override {
		Entity_PhysicsObject::OnCreate();

		SetName("Player");
		RegisterAsDrawnable(GetResourcePath("Models/Arms/FP_Arms.dae"));
		GetDrawnable().AnimatorInstance = _animator;
		_idleAnimation = GetDrawnable().ImportAnimation(GetResourcePath("Models/Arms/FP_Arms_Idle.dae"));
		_walkingAnimation = GetDrawnable().ImportAnimation(GetResourcePath("Models/Arms/FP_Arms_RifleWalk.dae"));
		_animator->PlayAnimation(_idleAnimation);
	};

	virtual void OnUpdate(float deltaTime) override {
		glm::vec3 velocity = GetVelocity();
		if (ae::Input::IsKeyPressed(ae::key::Up))
			velocity.x += (_speed * deltaTime);
		if (ae::Input::IsKeyPressed(ae::key::Down))
			velocity.x -= (_speed * deltaTime);
		if (ae::Input::IsKeyPressed(ae::key::Right))
			velocity.z += (_speed * deltaTime);
		if (ae::Input::IsKeyPressed(ae::key::Left))
			velocity.z -= (_speed * deltaTime);

		SetLinearVelocity(velocity);

		float horizontalSpeed = glm::length(glm::vec2(velocity.x, velocity.z));
		if (horizontalSpeed > 0.1f) {
			_animator->PlayAnimation(_walkingAnimation);
		}
		else {
			_animator->PlayAnimation(_idleAnimation);
		}
	};

	virtual void OnDestroy() override {};
private:
	memory::Ref<grapichs::Animator> _animator;
	grapichs::Animation* _idleAnimation = nullptr;
	grapichs::Animation* _walkingAnimation = nullptr;
	float _speed = 10.0f;
};