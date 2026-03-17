#include "ArchPch.h"
#include "Scene.h"
#include "ArchEngine/Core/Application.h"

#include "ArchEngine/Objects/Entity_Camera.h"

namespace ae {
	Scene::Scene(const std::string& name) : _name(name) {
		_sceneRenderer = memory::MakeScope<SceneRenderer>(this);
		_scenePhysics = memory::MakeScope<ScenePhysics>(this);
	}

	void Scene::DestroyEntity(EntityID id) {
		auto it = _entities.find(id);
		if (it != _entities.end()) {
			it->second->OnDestroy();
			_entities.erase(it);
		}
	}

	void Scene::Destroy() {
		Application::Get()->GetWindow().GetRenderContext().WaitDeviceIdle();
		for (auto& [id, entity] : _entities) { entity->OnDestroy(); }
		_entities.clear();
		_sceneRenderer->RemoveAllDrawnables();
		_scenePhysics->RemoveAllPhysicsBody();
	}

	Entity* Scene::CreateEntity(const std::string& typeName, EntityID id) {
		memory::Ref<Entity> newEntity = EntityFactory::Create(typeName);
		if (!newEntity) {
			std::cout << "Error: Entity type " << typeName << " not registered!" << std::endl;
			return nullptr;
		}
		_entities[id] = newEntity;
		_entityTypes[id] = typeName;
		newEntity->SetID(id);
		newEntity->SetScene(this);
		newEntity->OnCreate();
		return newEntity.Get();
	}

	Entity* Scene::GetEntity(EntityID id) {
		auto it = _entities.find(id);
		if (it != _entities.end()) {
			return it->second.Get();
		}
		throw std::runtime_error("Entity with ID " + std::to_string(id) + " not found.");
	}

	std::string Scene::GetEntityType(EntityID id) {
		auto it = _entityTypes.find(id);
		if (it != _entityTypes.end()) {
			return it->second;
		}
		throw std::runtime_error("Entity Type with ID " + std::to_string(id) + " not registered.");
	}

	void Scene::OnEditorUpdate(const memory::Ref<grapichs::Camera>& cam, float deltaTime) {
		for (auto& [handle, entity] : _entities) {
			entity->OnUpdate(deltaTime);
		}
		_scenePhysics->Step(deltaTime, _entities);
		_sceneRenderer->RenderScene(cam, _entities);
	}

	void Scene::OnRuntimeUpdate(float deltaTime) {
		for (auto& [handle, entity] : _entities) {
			entity->OnUpdate(deltaTime);
		}
		_scenePhysics->Step(deltaTime, _entities);

		memory::Ref<grapichs::Camera> mainCamera = nullptr;
		auto cameraEntities = Group<Entity_Camera>();
		if (!cameraEntities.empty()) {
			if (Entity_Camera* camEntity = dynamic_cast<Entity_Camera*>(GetEntity(cameraEntities[0]))) {
				mainCamera = camEntity->GetCamera();
			}
		}
		if (mainCamera) {
			_sceneRenderer->RenderScene(mainCamera, _entities);
		}
		else {
			Logger_app::warn("No active camera found in the scene!");
		}
	}
}