#pragma once

#include <ArchEngine/Objects/Entity.h>
#include <ArchEngine/Core/Memory.h>
#include <ArchEngine/Scene/Scene.h>
#include <ArchEngine/Grapichs/Animator.h>
#include <ArchEngine/Grapichs/Animation.h>
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
		_danceAnimation = GetDrawnable().ImportAnimation("Resources/Models/robot/HipHopDancing.dae");
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
		
		if (ae::Input::IsKeyJustPressed(ae::key::U)) {
			_animator->PlayAnimation(_danceAnimation);
		}

		SetLinearVelocity(velocity);
	};

	virtual void OnDestroy() override {};
private:
	ae::memory::Ref<ae::grapichs::Animator> _animator;
	ae::grapichs::Animation* _danceAnimation = nullptr;
	float _speed = 50.0f;
};