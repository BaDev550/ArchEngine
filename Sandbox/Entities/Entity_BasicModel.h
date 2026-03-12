#pragma once

#include <ArchEngine/Objects/Entity.h>
#include <ArchEngine/Scene/Scene.h>

class Entity_BasicModel : public ae::Entity {
public:
	REGISTER_ENTITY(Entity_BasicModel);

	Entity_BasicModel() {}
	Entity_BasicModel(const std::string& path) {
		_path = path;
	}

	virtual void OnCreate() override {
		SetName("Basic Entity Model");
		RegisterAsDrawnable(_path);
	};

	virtual void OnUpdate(float deltaTime) override {};
	virtual void OnDestroy() override {};
private:
	std::string _path;
};