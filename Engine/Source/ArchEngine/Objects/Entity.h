#pragma once
#include "ArchEngine/Utilities/UUID.h"
#include "ArchEngine/Utilities/Math.h"
#include "ArchEngine/Core/Memory.h"
#include "EntityFactory.h"


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

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
		glm::vec3 Scale = glm::vec3(1.0f);

		glm::mat4 Mat4() const {
			return glm::translate(glm::mat4(1.0f), Position)
				* glm::toMat4(Rotation)
				* glm::scale(glm::mat4(1.0f), Scale);
		}

		glm::vec3 GetEulerRotation() const { return EulerRotation; }
		glm::quat GetRotation() const { return Rotation; }

		void SetEulerRotation(const glm::vec3& rotation) {
			EulerRotation = rotation;
			Rotation = glm::quat(glm::radians(rotation));
		}
		void SetRotation(const glm::quat& quat) {
			Rotation = quat;
			EulerRotation = glm::degrees(glm::eulerAngles(quat));
		}
		void SetTransform(const glm::mat4& transform) {
			math::DecomposeTransform(transform, Position, Rotation, Scale);
			EulerRotation = glm::degrees(glm::eulerAngles(Rotation));
		}
	private:
		glm::quat Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 EulerRotation = glm::vec3(0.0f);
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
		const glm::quat GetRotation() const { return _transform.GetRotation(); }
		const glm::vec3 GetEulerRotation() const { return _transform.GetEulerRotation(); }
		const glm::vec3 GetScale() const { return _transform.Scale; }
		const glm::mat4 GetTransformMatrix() const { return _transform.Mat4(); }
		const std::string GetName() const { return _identifier.Name; }
		Drawnable& GetDrawnable();
		PhysicsBody& GetPhysicsBody();
		bool HasPhysicsBody();
		bool IsDrawnable();

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
		void SetRotation(const glm::quat& quat);
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