#pragma once
#include "Texture.h"
#include "Shader.h"
#include "ShaderTypes.h"
#include "Buffer.h"
#include "DescriptorManager.h"
#include "ArchEngine/Utilities/DataBuffer.h"
#include "ArchEngine/AssetManager/Asset.h"

#include <glm/glm.hpp>

namespace ae::grapichs {
	class Material : public memory::RefCounted {
	public:
		Material(memory::Ref<Shader>& shader);
		~Material();

		void Build();
		void Bind(vk::CommandBuffer cmd, vk::PipelineLayout layout);

		void Set(const std::string& name, float value);
		void Set(const std::string& name, int value);
		void Set(const std::string& name, const glm::vec2& value);
		void Set(const std::string& name, const glm::vec3& value);
		void Set(const std::string& name, const memory::Ref<Texture2D>& value);

		template<typename T>
		void SetData(const std::string& name, const T& value) {
			auto decl = FindUniformDeclaration(name);
			_materialData.Write((uint8_t*)&value, decl->GetSize(), decl->GetOffset());
		}

		template<typename T>
		T& GetData(const std::string& name) {
			auto decl = FindUniformDeclaration(name);
			auto& buffer = _materialData;
			return buffer.Read<T>(decl->GetOffset());
		}
		const std::vector<vk::DescriptorSet>& GetDescriptorSets() const { return _descriptorSets; }
	private:
		const ShaderUniform* FindUniformDeclaration(const std::string& name) const;
		memory::Ref<Shader> _shader;
		memory::Ref<Buffer> _materialBuffer;
		memory::Ref<DescriptorManager> _descriptorManager;
		std::vector<vk::DescriptorSet> _descriptorSets;
		DataBuffer _materialData;
	};

	class MaterialAsset : public Asset {
	public:
		MaterialAsset(const memory::Ref<Material>& material) : _material(material) {}
		void SetAlbedoTexture(const AssetHandle& textureHandle);
		void SetNormalTexture(const AssetHandle& textureHandle);
		bool HasNormalTexture() const { return _textureHandles.NormalHandle; }
		memory::Ref<Material>& GetMaterial() { return _material; }
		static AssetType GetStaticAssetType() { return AssetType::Material; }
		virtual AssetType GetAssetType() const override { return GetStaticAssetType(); }
	private:
		struct TextureHandles {
			AssetHandle AlbedoHandle;
			AssetHandle NormalHandle;
		} _textureHandles;
		memory::Ref<Material> _material;
	};
}