#pragma once
#include "Texture.h"
#include "Shader.h"
#include "ShaderTypes.h"
#include "Buffer.h"
#include "DescriptorManager.h"
#include "ArchEngine/Utilities/DataBuffer.h"

#include <glm/glm.hpp>

namespace ae::grapichs {
	const static std::string ALBEDO_TEXTURE_UNIFORM_NAME = "uAlbedoTexture";
	const static std::string NORMAL_TEXTURE_UNIFORM_NAME = "uNormTexture";
	const static uint32_t MATERIAL_DESCRIPTOR_LAYOUT_SET_INDEX = 1;

	class Material : public memory::RefCounted {
	public:
		Material(memory::Ref<Shader>& shader);
		~Material();

		void Build();
		void Bind(vk::CommandBuffer cmd, vk::PipelineLayout layout);

		void SetAlbedoTexture(const memory::Ref<Texture2D>& texture) { Set(ALBEDO_TEXTURE_UNIFORM_NAME, texture); }
		void SetNormalTexture(const memory::Ref<Texture2D>& texture) { Set(NORMAL_TEXTURE_UNIFORM_NAME, texture); }
		bool HasNormalTexture() const { return _data.Normal; }

		template<typename T>
		void Set(const std::string& name, const T& value) {
			auto decl = FindUniformDeclaration(name);
			_materialData.Write((uint8_t*)&value, decl->GetSize(), decl->GetOffset());
		}

		template<typename T>
		T& Get(const std::string& name) {
			auto decl = FindUniformDeclaration(name);
			auto& buffer = _materialData;
			return buffer.Read<T>(decl->GetOffset());
		}

		const std::vector<vk::DescriptorSet>& GetDescriptorSets() const { return _descriptorSets; }
	private:
		void Set(const std::string& name, float value);
		void Set(const std::string& name, int value);
		void Set(const std::string& name, const glm::vec2& value);
		void Set(const std::string& name, const glm::vec3& value);
		void Set(const std::string& name, const memory::Ref<Texture2D>& value);

		const ShaderUniform* FindUniformDeclaration(const std::string& name) const;
		struct TextureMaps {
			memory::Ref<Texture2D> Albedo;
			memory::Ref<Texture2D> Normal;
		} _data;
		memory::Ref<Shader> _shader;
		memory::Ref<Buffer> _materialBuffer;
		memory::Ref<DescriptorManager> _descriptorManager;
		std::vector<vk::DescriptorSet> _descriptorSets;
		DataBuffer _materialData;
	};
}