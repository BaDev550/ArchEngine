#pragma once

#include "Entity.h"
#include "ArchEngine/Grapichs/Light.h"

namespace ae {
	class Entity_DirectionalLight : public Entity {
	public:
		REGISTER_ENTITY(Entity_DirectionalLight);

		Entity_DirectionalLight() {}
		Entity_DirectionalLight(float intensity) {
			_handle.Intensity = intensity;
		}

		virtual void OnCreate() override { SetName("Sun Light"); };
		grapichs::DirectionalLight& GetHandle() { return _handle; }
	private:
		grapichs::DirectionalLight _handle;
	};
}