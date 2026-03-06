#pragma once

#include <ArchEngine/Objects/Entity.h>
#include <ArchEngine/Scene/Scene.h>

class BasicObject : public ae::Entity {
public:
	virtual void OnCreate() override {
		SetName("Basic Object");
		GetScene()->GetRenderer().AddDrawnable(GetID(), "Resources/Models/mario_2/mario_2.obj");
	};

	virtual void OnUpdate(float deltaTime) override {};
	virtual void OnDestroy() override {};
};