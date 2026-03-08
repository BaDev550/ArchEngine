#include "ArchPch.h"
#include "Scene.h"

namespace ae {
	Scene::Scene() {
		_sceneRenderer = memory::MakeScope<SceneRenderer>();
		_scenePhysics = memory::MakeScope<ScenePhysics>(this);
	}

	void Scene::DestroyEntity(EntityID id) {
		auto it = _entities.find(id);
		if (it != _entities.end()) {
			it->second->OnDestroy();
			_entities.erase(it);
		}
	}

	Entity& Scene::GetEntity(EntityID id) {
		auto it = _entities.find(id);
		if (it != _entities.end()) {
			return *it->second;
		}
		throw std::runtime_error("Entity with ID " + std::to_string(id) + " not found.");
	}

	void Scene::OnEditorUpdate(const memory::Ref<grapichs::Camera>& cam, float deltaTime) {
		_scenePhysics->Step(deltaTime, _entities);
		_sceneRenderer->RenderScene(cam, _entities);
	}

	void Scene::OnRuntimeUpdate(float deltaTime) {
		
	}
}