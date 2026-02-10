#pragma once
#include <unordered_map>
#include "Shader.h"

#include "ArchEngine/Core/Memory.h"

namespace ae::grapichs {
	class ShaderLibrary {
	public:
		memory::Ref<Shader>& AddShader(std::string_view name, const std::string& vertexPath, const std::string& fragmentPath);
		memory::Ref<Shader>& GetShader(std::string_view name);
		memory::Ref<Shader>& ReloadShader(std::string_view name);
		const std::unordered_map<std::string, memory::Ref<Shader>>& GetShaders() const;
	private:
		std::unordered_map<std::string, memory::Ref<Shader>> _shaders;
	};
}