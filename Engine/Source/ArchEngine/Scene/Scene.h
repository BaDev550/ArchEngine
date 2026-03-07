#pragma once

#include "ArchEngine/Grapichs/Camera.h"
#include "ArchEngine/Objects/Entity.h"
#include "ArchEngine/Core/Memory.h"
#include "ArchEngine/Scene/SceneRenderer.h"
#include "ArchEngine/Scene/ScenePhysics.h"

#include <unordered_map>

namespace ae {
	class Scene : public memory::RefCounted {
	public:
		Scene();

		template<typename T, typename... Args>
		T* CreateEntity(Args&&... args);
		void DestroyEntity(EntityID id);
		Entity& GetEntity(EntityID id);
		SceneRenderer& GetRenderer() { return *_sceneRenderer; }
		ScenePhysics& GetPhysics() { return *_scenePhysics; }
		std::unordered_map<EntityID, memory::Ref<Entity>>& GetEntities() { return _entities; }

		void OnEditorUpdate(const memory::Ref<grapichs::Camera>& cam, float deltaTime);
		void OnRuntimeUpdate(float deltaTime);
	private:
		std::unordered_map<EntityID, memory::Ref<Entity>> _entities;
		memory::Scope<SceneRenderer> _sceneRenderer = nullptr;
		memory::Scope<ScenePhysics> _scenePhysics = nullptr;
	};

	template<typename T, typename ...Args>
	T* Scene::CreateEntity(Args && ...args)
	{
		EntityID id = EntityID();
		static_assert(std::is_base_of<Entity, T>::value, "T must be derived from Entity");
		_entities[id] = memory::Ref<T>::Create(std::forward<Args>(args)...);
		_entities[id]->SetID(id);
		_entities[id]->SetScene(this);
		_entities[id]->OnCreate();
		return static_cast<T*>(_entities[id].Get());
	}
}