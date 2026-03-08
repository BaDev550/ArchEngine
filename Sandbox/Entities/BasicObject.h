#pragma once

#include <ArchEngine/Objects/Entity.h>
#include <ArchEngine/Scene/Scene.h>

class BasicObject : public ae::Entity {
public:
	REGISTER_ENTITY(BasicObject);
	BasicObject() {}

	virtual void OnCreate() override {
		SetName("Basic Object");
		RegisterAsDrawnable("Resources/Models/mario_2/mario_2.obj");
	};

	virtual void OnUpdate(float deltaTime) override {};
	virtual void OnDestroy() override {};
};