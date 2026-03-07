#include "ArchPch.h"
#include "Material.h"
#include "Renderer.h"

#include "ArchEngine/AssetManager/AssetManager.h"

namespace ae::grapichs {
    const static std::string ALBEDO_TEXTURE_UNIFORM_NAME = "uAlbedoTexture";
    const static std::string NORMAL_TEXTURE_UNIFORM_NAME = "uNormTexture";
    const static uint32_t MATERIAL_DESCRIPTOR_LAYOUT_SET_INDEX = 1;

    void MaterialAsset::SetAlbedoTexture(const AssetHandle& textureHandle) {
        if (textureHandle == INVALID_ASSET_HANDLE)
            return;
		_textureHandles.AlbedoHandle = textureHandle;
		const memory::Ref<Texture2D>& texture = AssetManager::GetAsset<Texture2D>(textureHandle);
		_material->Set(ALBEDO_TEXTURE_UNIFORM_NAME, texture);
    }

    void MaterialAsset::SetNormalTexture(const AssetHandle& textureHandle) {
        if (textureHandle == INVALID_ASSET_HANDLE)
            return;
        _textureHandles.NormalHandle = textureHandle;
        const memory::Ref<Texture2D>& texture = AssetManager::GetAsset<Texture2D>(textureHandle);
        _material->Set(NORMAL_TEXTURE_UNIFORM_NAME, texture);
    }

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

            //_descriptorSets.resize(Renderer::MaxFramesInFlight);
            //for (uint32_t i = 0; i < Renderer::MaxFramesInFlight; i++) {
            //    _descriptorSets[i] = _descriptorManager->Allocate(layout);
            //}

            for (const auto& [name, decl] : _descriptorManager->GetInputDeclarations()) {
                if (decl.Type == ShaderReflectionDataType::Sampler2D) {
                    _descriptorManager->WriteInput(name, Renderer::GetWhiteTexture());
                }
                else if (decl.Type == ShaderReflectionDataType::UniformBuffer) {
                    if (_materialBuffer)
                        _descriptorManager->WriteInput("uMaterial", _materialBuffer);
                }
            }
            _descriptorManager->Bake();
        }
    }

    void Material::Bind(vk::CommandBuffer cmd, vk::PipelineLayout layout)
    {
        if (_materialBuffer && _materialData.Data)
			_materialBuffer->Write(_materialData.Data, _materialData.Size);
        //vk::DescriptorSet descriptorSet = _descriptorSets.at(Renderer::GetFrameIndex());
        //if (descriptorSet)
        //    cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, 1, descriptorSet, {});
		_descriptorManager->BindSets(cmd, layout);
    }

    void Material::Set(const std::string& name, float value)
    {
        SetData<float>(name, value);
        _descriptorManager->Bake();
    }

    void Material::Set(const std::string& name, int value)
    {
        SetData<float>(name, value);
        _descriptorManager->Bake();
    }

    void Material::Set(const std::string& name, const glm::vec2& value)
    {
        SetData<glm::vec2>(name, value);
        _descriptorManager->Bake();
    }

    void Material::Set(const std::string& name, const glm::vec3& value)
    {
        SetData<glm::vec3>(name, value);
        _descriptorManager->Bake();
    }

    void Material::Set(const std::string& name, const memory::Ref<Texture2D>& value)
    {
        _descriptorManager->WriteInput(name, value);
        _descriptorManager->Bake();
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