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

	void Entity::RegisterAsDrawnable() {
		SetRenderHandle(GetScene()->GetRenderer().AddDrawnable(GetID()));
	}

	void Entity::RegisterAsDrawnable(const std::string& modelPath) {
		RegisterAsDrawnable();
		GetDrawnable().ImportStaticMesh(modelPath);
	}

	void Entity::RegisterAsPhysicsBody() {
		SetPhysicsHandle(GetScene()->GetPhysics().CreatePhysicsBody(GetID()));
	}
}