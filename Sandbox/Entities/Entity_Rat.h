#pragma once

#include "PhysicsObject.h"
#include <ArchEngine/Objects/Entity_Camera.h>

class Entity_Rat : public PhysicsObject {
public:
	REGISTER_ENTITY(Entity_Rat);

	enum eRatStatus : uint8_t {
		eRatStatus_Happy = 0,
		eRatStatus_Angry
	};

	Entity_Rat() {
		_type = ae::PhysicsMotionType::Dynamic;
		_boxColliderExtent = glm::vec3(0.2f, 0.5f, 0.25f);
		_boxColliderOffset = glm::vec3(0.0f, 0.6f, 0.0f);
	}

	virtual void OnCreate() override {
		SetPosition({ 0.0f, 10.0f, 0.0f });
		SetRotation({ 90.0f, 90.0f, -180.0f });
		SetScale({ 0.05f, 0.05f, 0.05f });
		PhysicsObject::OnCreate();
		SetName("Rat");
		RegisterAsDrawnable("Resources/Models/rat/scene.gltf");
	};

	virtual void OnUpdate(float deltaTime) override {
		if (ae::Input::IsKeyJustPressed(ae::key::K))
			_ratstat = eRatStatus_Angry;
		if (ae::Input::IsKeyJustPressed(ae::key::M))
			_ratstat = eRatStatus_Happy;

		ImGui::Begin("Rat Debug Menu");
		ImGui::Text("Rat stat: %s", GetRatStatus().c_str());
		ImGui::End();

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
	};
	virtual void OnDestroy() override {};
private:
	eRatStatus _ratstat = eRatStatus_Happy;
	float _speed = 50.0f;

	std::string GetRatStatus() const {
		switch (_ratstat) {
		case eRatStatus_Happy: return "Happy";
		case eRatStatus_Angry: return "Angry";
		}
		return "undefined";
	}
};