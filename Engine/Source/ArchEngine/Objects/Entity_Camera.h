#pragma once

#include "Entity.h"
#include "ArchEngine/Grapichs/Camera.h"

namespace ae {
	class Entity_Camera : public Entity {
	public:
		REGISTER_ENTITY(Entity_Camera);

		Entity_Camera() {}

		virtual void OnCreate() override {
			SetName("Camera");
		};

		memory::Ref<grapichs::Camera>& GetHandle() { return _handle; }

		virtual void OnUpdate(float deltaTime) override {};
		virtual void OnDestroy() override {};
	protected:
		memory::Ref<grapichs::Camera> _handle;
	};
}