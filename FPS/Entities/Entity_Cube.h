#pragma once

#include <ArchEngine/Objects/Entity.h>
#include <ArchEngine/Objects/Entity_PhysicsObject.h>
#include "Common/Defines.h"

using namespace ae;
class Entity_Cube : public Entity_PhysicsObject {
public:
	REGISTER_ENTITY(Entity_Cube);

	Entity_Cube() {
		_type = PhysicsMotionType::Static;
		_boxColliderExtent = { 10.0f, 0.1f, 10.0f };
		_boxColliderOffset = { 0.0f, 0.0f, 0.0f };
	}

	virtual void OnCreate() override {
		SetName("Cube");
		Entity_PhysicsObject::OnCreate();
		RegisterAsDrawnable(GetEngineResourcePath("Models/CheckerBox/Cube.obj"));
	};
	virtual void OnUpdate(float deltaTime) override {};
	virtual void OnDestroy() override {};
};