#include "ArchPch.h"
#include "Material.h"
#include "Renderer.h"

namespace ae::grapichs {
    Material::Material(memory::Ref<Shader>& shader)
    {
		_shader = shader;
		_descriptorManager = memory::Ref<DescriptorManager>::Create(_shader);
        _descriptorManager->Invalidate();
        Build();
    }

    Material::~Material() {
        _materialData.Release();
    }

    void Material::Build()
    {
        if (_shader->GetDescriptorLayout(MATERIAL_DESCRIPTOR_LAYOUT_SET_INDEX)) {
            auto& layout = _shader->GetDescriptorLayout(MATERIAL_DESCRIPTOR_LAYOUT_SET_INDEX);
            const auto& shaderBuffers = _shader->GetShaderBuffers();

            if (shaderBuffers.size() > 0) {
                uint32_t size = 0;
                for (auto [name, buffer] : shaderBuffers)
                    size += buffer.Size;
                _materialData.Allocate(size);
                _materialData.ZeroInitialize();

                _materialBuffer = memory::Ref<Buffer>::Create(
                    size, 
                    vk::BufferUsageFlagBits::eUniformBuffer, 
                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
                _materialBuffer->Map();
            }

            _descriptorSets.resize(Renderer::MaxFramesInFlight);
            for (uint32_t i = 0; i < Renderer::MaxFramesInFlight; i++) {
                _descriptorSets[i] = _descriptorManager->Allocate(layout);
            }

            for (const auto& [name, decl] : _descriptorManager->GetInputDeclarations()) {
                if (decl.Type == ShaderReflectionDataType::Sampler2D) {
                    _descriptorManager->WriteInput(name, Renderer::GetWhiteTexture());
                }
                else if (decl.Type == ShaderReflectionDataType::UniformBuffer) {
                    if (_materialBuffer)
                        _descriptorManager->WriteInput("uMaterial", _materialBuffer);
                }
            }
        }
    }

    void Material::Bind(vk::CommandBuffer cmd, vk::PipelineLayout layout)
    {
        if (_materialBuffer && _materialData.Data)
			_materialBuffer->Write(_materialData.Data, _materialData.Size);
		_descriptorManager->UpdateSets(cmd, layout);
    }

    void Material::Set(const std::string& name, float value)
    {
        Set<float>(name, value);
    }

    void Material::Set(const std::string& name, int value)
    {
        Set<float>(name, value);
    }

    void Material::Set(const std::string& name, const glm::vec2& value)
    {
        Set<glm::vec2>(name, value);
    }

    void Material::Set(const std::string& name, const glm::vec3& value)
    {
        Set<glm::vec3>(name, value);
    }

    void Material::Set(const std::string& name, const memory::Ref<Texture2D>& value)
    {
        _descriptorManager->WriteInput(name, value);
    }

    const ShaderUniform* Material::FindUniformDeclaration(const std::string& name) const
    {
        const auto& shaderBuffers = _shader->GetShaderBuffers();
        for (const auto& [bufferName, buffer] : shaderBuffers) {
            auto it = buffer.Uniforms.find(name);
            if (it != buffer.Uniforms.end())
                return &it->second;
        }

        return nullptr;
    }
}