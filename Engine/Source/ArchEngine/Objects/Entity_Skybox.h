#pragma once

#include "Entity.h"
#include <filesystem>
#include "ArchEngine/Grapichs/Enviroment.h"
#include "ArchEngine/AssetManager/AssetManager.h"

namespace ae {
	class Entity_Skybox : public Entity {
	public:
		REGISTER_ENTITY(Entity_Skybox);

		Entity_Skybox() {}
		Entity_Skybox(const std::filesystem::path& path) : _skyBoxPath(path.string()) {
			LoadSkybox();
		}
		AssetHandle GetEnviromentMapHandle() const { return _enviromentMapHandle; }

		virtual void OnCreate() override {
			SetName("Skybox");
		};

		virtual void OnUpdate(float deltaTime) override {};
		virtual void OnDestroy() override {};
	private:
		void LoadSkybox() {
			_enviromentMapHandle = AssetManager::ImportAsset(_skyBoxPath);
		}
		AssetHandle _enviromentMapHandle;
		std::string _skyBoxPath;
	};
}