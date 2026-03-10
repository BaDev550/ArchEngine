#pragma once

#include "ArchEngine/Grapichs/Camera.h"
#include "ArchEngine/Objects/Entity.h"
#include "ArchEngine/Core/Memory.h"
#include "ArchEngine/Scene/SceneRenderer.h"
#include "ArchEngine/Scene/ScenePhysics.h"

#include <unordered_map>

namespace ae {
	static std::string RemoveClassOrStructSubfix(std::string name) {
		if (name.find("class ") == 0) name.erase(0, 6);
		if (name.find("struct ") == 0) name.erase(0, 7);
		return name;
	}

	class Scene : public memory::RefCounted {
	public:
		Scene(const std::string& name = "undefined_scene");

		template<typename T, typename... Args>
		T* CreateEntity(Args&&... args);
		Entity* CreateEntity(const std::string& typeName, EntityID id = EntityID());
		void DestroyEntity(EntityID id);
		void Destroy();
		Entity* GetEntity(EntityID id);
		std::string GetEntityType(EntityID id);
		std::string GetName() const { return _name; }
		SceneRenderer& GetRenderer() { return *_sceneRenderer; }
		ScenePhysics& GetPhysics() { return *_scenePhysics; }
		std::unordered_map<EntityID, memory::Ref<Entity>>& GetEntities() { return _entities; }
		std::unordered_map<EntityID, std::string>& GetEntityTypes() { return _entityTypes; }

		void OnEditorUpdate(const memory::Ref<grapichs::Camera>& cam, float deltaTime);
		void OnRuntimeUpdate(float deltaTime);

		template<typename T>
		std::vector<EntityID> Group() const {
			std::vector<EntityID> result;
			for (const auto& [handle, e] : _entities) {
				if (e.As<T>())
					result.push_back(e->GetID());
			}
			return result;
		}
	private:
		std::string _name;
		std::unordered_map<EntityID, memory::Ref<Entity>> _entities;
		std::unordered_map<EntityID, std::string> _entityTypes;
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
		_entityTypes[id] = RemoveClassOrStructSubfix(typeid(T).name());
		return static_cast<T*>(_entities[id].Get());
	}
}