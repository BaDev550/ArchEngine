#include "ShaderLibrary.h"

namespace ae::grapichs {
    memory::Ref<Shader>& ShaderLibrary::AddShader(std::string_view name, const std::string& vertexPath, const std::string& fragmentPath) {
        return _shaders[name.data()] = memory::Ref<Shader>::Create(vertexPath, fragmentPath);
    }

    memory::Ref<Shader>& ShaderLibrary::GetShader(std::string_view name)
    {
        std::string nameStr = name.data();
        CHECKF(_shaders.find(nameStr) == _shaders.end(), "Failed to find shader");
        return _shaders.at(nameStr);
    }

    memory::Ref<Shader>& ShaderLibrary::ReloadShader(std::string_view name)
    {
        //_shaders.at(name.data())->Reload(); todo - add a reload function to shaders
        return _shaders.at(name.data());
    }

    const std::unordered_map<std::string, memory::Ref<Shader>>& ShaderLibrary::GetShaders() const
    {
        return _shaders;
    }
}