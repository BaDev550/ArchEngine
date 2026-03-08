#pragma once
#include "ArchEngine/Core/Memory.h"

namespace ae {
	class Scene;
	class SceneSerializer {
	public:
		SceneSerializer(const memory::Ref<Scene>& scene);
		void Serialize(const std::string& filepath);
		bool Deserialize(const std::string& filepath);
	private:
		memory::Ref<Scene> _scene;
	};
}