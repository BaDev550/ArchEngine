#pragma once
#include "ArchEngine/Utilities/UUID.h"
#include "ArchEngine/Core/Memory.h"
#include "EntityFactory.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <yaml-cpp/yaml.h>

namespace ae {
	class Scene;
	struct Drawnable;
	struct PhysicsBody;
	using EntityID = UUID;

	struct RenderHandle {
		size_t index = SIZE_MAX;
		bool IsValid() const { return index != SIZE_MAX; }
	};

	struct PhysicsHandle {
		size_t index = SIZE_MAX;
		bool IsValid() const { return index != SIZE_MAX; }
	};

	struct TransformComponent {
		glm::vec3 Position = glm::vec3(0.0f);
		glm::vec3 Rotation = glm::vec3(0.0f);
		glm::vec3 Scale = glm::vec3(1.0f);

		glm::mat4 Mat4() const {
			glm::mat4 translation = glm::translate(glm::mat4(1.0f), Position);
			glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), Scale);
			return translation * rotationY * rotationX * rotationZ * scale;
		}
	};

	struct IdentifierComponent {
		std::string Name = "EMPTY_NAME";
	};

	class Entity : public memory::RefCounted {
	public:
		Entity() = default;
		virtual ~Entity() = default;
		Entity(EntityID id) : _id(id) {}
		EntityID GetID() const { return _id; }
		const glm::vec3 GetPosition() const { return _transform.Position; }
		const glm::vec3 GetRotation() const { return _transform.Rotation; }
		const glm::vec3 GetScale() const { return _transform.Scale; }
		const glm::mat4 GetTransformMatrix() const { return _transform.Mat4(); }
		const std::string GetName() const { return _identifier.Name; }
		Drawnable& GetDrawnable();
		PhysicsBody& GetPhysicsBody();
		bool HasPhysicsBody();

		virtual void OnCreate() {};
		virtual void OnUpdate(float deltaTime) {};
		virtual void OnDestroy() {};
		virtual void OnSerialize(YAML::Emitter& out) {};
		virtual void OnDeserialize(YAML::Node& data) {};

		void RegisterAsDrawnable();
		void RegisterAsDrawnable(const std::string& modelPath);
		void RegisterAsPhysicsBody();
		void SetID(EntityID id) { _id = id; }
		void SetScene(Scene* scene) { _scene = scene; }
		void SetPosition(const glm::vec3& position);
		void SetRotation(const glm::vec3& rotation);
		void SetScale(const glm::vec3& scale);
		void SetName(const std::string& name) { _identifier.Name = name; }
		void SetRenderHandle(const RenderHandle& handle) {
			if (!_renderHandle.IsValid())
				_renderHandle = handle;
		}
		void SetPhysicsHandle(const PhysicsHandle& handle) {
			if (!_physicsHandle.IsValid())
				_physicsHandle = handle;
		}

		TransformComponent& GetTransform() { return _transform; }
		IdentifierComponent& GetIdentifier() { return _identifier; }
		RenderHandle GetRenderHandle() const { return _renderHandle; }
		PhysicsHandle GetPhysicsHandle() const { return _physicsHandle; }
		Scene* GetScene() { return _scene; }
	private:
		Scene* _scene = nullptr;
		RenderHandle _renderHandle;
		PhysicsHandle _physicsHandle;

		EntityID _id = 0;
		TransformComponent _transform;
		IdentifierComponent _identifier;
	};
}