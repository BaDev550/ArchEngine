#include "ArchPch.h"
#include "Entity.h"
#include "ArchEngine/Scene/Scene.h"

namespace ae {
	Drawnable& Entity::GetDrawnable() {
		return _scene->GetRenderer().GetDrawnable(_renderHandle);
	}

	void Entity::RegisterAsDrawnable() {
		SetRenderHandle(GetScene()->GetRenderer().AddDrawnable(GetID()));
	}

	void Entity::RegisterAsDrawnable(const std::string& modelPath) {
		RegisterAsDrawnable();
		GetDrawnable().ImportStaticMesh(modelPath);
	}
}