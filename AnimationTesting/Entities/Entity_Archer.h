#pragma once

#include <ArchEngine/Objects/Entity.h>
#include <ArchEngine/Scene/Scene.h>
#include <ArchEngine/Grapichs/Animator.h>

class Entity_Archer : public ae::Entity {
public:
	REGISTER_ENTITY(Entity_Archer);

	Entity_Archer() {}

	virtual void OnCreate() override {
		SetName("EntityArcher");
		RegisterAsDrawnable("Resources/Models/robot/ElyByKAtienza.dae");
	};

	virtual void OnUpdate(float deltaTime) override {};
	virtual void OnDestroy() override {};
};