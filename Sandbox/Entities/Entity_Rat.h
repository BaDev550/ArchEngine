#pragma once

#include "PhysicsObject.h"

class Entity_Rat : public PhysicsObject {
public:
	REGISTER_ENTITY(Entity_Rat);

	enum eRatStatus : uint8_t {
		eRatStatus_Happy = 0,
		eRatStatus_Angry
	};

	Entity_Rat() {
		_type = ae::PhysicsMotionType::Dynamic;
		_boxColliderExtent = glm::vec3(0.3f, 0.1f, 0.3f);
	}

	virtual void OnCreate() override {
		SetPosition({ 0.0f, 10.0f, 0.0f });
		SetRotation({ -90.0f, 0.0f, 90.0f });
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
	};
	virtual void OnDestroy() override {};
private:
	eRatStatus _ratstat = eRatStatus_Happy;

	std::string GetRatStatus() const {
		switch (_ratstat) {
		case eRatStatus_Happy: return "Happy";
		case eRatStatus_Angry: return "Angry";
		}
		return "undefined";
	}
};