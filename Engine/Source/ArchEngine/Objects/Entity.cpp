#include "ArchPch.h"
#include "Entity.h"
#include "ArchEngine/Scene/Scene.h"

namespace ae {
	Drawnable& Entity::GetDrawnable() {
		return _scene->GetRenderer().GetDrawnable(_renderHandle);
	}

	PhysicsBody& Entity::GetPhysicsBody() {
		return _scene->GetPhysics().GetPhysicsBody(_physicsHandle);
	}

	bool Entity::HasPhysicsBody() { return _physicsHandle.IsValid(); }

	void Entity::RegisterAsDrawnable() {
		SetRenderHandle(GetScene()->GetRenderer().AddDrawnable(GetID()));
	}

	bool Entity::IsDrawnable() {
		return _renderHandle.IsValid();
	}

	void Entity::RegisterAsDrawnable(const std::string& modelPath) {
		RegisterAsDrawnable();
		GetDrawnable().ImportMesh(modelPath);
	}

	void Entity::RegisterAsPhysicsBody() {
		SetPhysicsHandle(GetScene()->GetPhysics().CreatePhysicsBody(GetID()));
	}

	void Entity::SetPosition(const glm::vec3& position) { 
		_transform.Position = position; 
	}

	void Entity::SetRotation(const glm::quat& quat) {
		_transform.SetRotation(quat);
	}

	void Entity::SetRotation(const glm::vec3& rotation) { 
		_transform.SetEulerRotation(rotation);
	}

	void Entity::SetScale(const glm::vec3& scale) { 
		_transform.Scale = scale; 
	}
}