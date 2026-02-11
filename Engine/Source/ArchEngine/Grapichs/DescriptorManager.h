#pragma once
#include "ShaderTypes.h"
#include "ArchEngine/Core/Memory.h"

#include "Buffer.h"
#include "Texture.h"
#include "Shader.h"
#include <vector>

namespace ae::grapichs {
	struct RenderPassInputDeclaration {
		ShaderReflectionDataType Type = ShaderReflectionDataType::None;
		uint32_t Set = 0;
		uint32_t Binding = 0;
		std::string Name;
	};

	class ResourceBinding {
	public:
		ShaderReflectionDataType _type = ShaderReflectionDataType::None;
		std::vector<memory::Ref<memory::RefCounted>> _data;

		ResourceBinding() : _data(std::vector<memory::Ref<memory::RefCounted>>(1, nullptr)) {}
		ResourceBinding(memory::Ref<Buffer> buffer) : _data(std::vector<memory::Ref<memory::RefCounted>>(1, buffer)), _type(ShaderReflectionDataType::UniformBuffer) {}
		ResourceBinding(memory::Ref<Texture2D> texture) : _data(std::vector<memory::Ref<memory::RefCounted>>(1, texture)), _type(ShaderReflectionDataType::Sampler2D) {}

		void Set(memory::Ref<Buffer> buffer, uint32_t index = 0) {
			_data[index] = buffer;
			_type = ShaderReflectionDataType::UniformBuffer;
		}
		void Set(memory::Ref<Texture2D> texture, uint32_t index = 0) {
			_data[index] = texture;
			_type = ShaderReflectionDataType::Sampler2D;
		}
	};

	class DescriptorManager {
	public:
		DescriptorManager() = default;
		DescriptorManager(memory::Ref<Shader>& shader);
		~DescriptorManager();

		void WriteInput(std::string_view name, memory::Ref<Buffer> buffer);
		void WriteInput(std::string_view name, memory::Ref<Texture2D> texture);

		void UpdateSets(vk::CommandBuffer cmd, vk::PipelineLayout layout);
		void Invalidate();

		vk::DescriptorSet Allocate(vk::DescriptorSetLayout layout);
		vk::DescriptorSet GetDescriptorSet(uint32_t setIndex) { return _descriptorSets[0][setIndex]; }
		vk::DescriptorPool GetDescriptorPool() const { return _descriptorPool; }
		std::vector<vk::DescriptorSet> GetDescriptorSets() { return _descriptorSets[0]; } // Get frame index
		const std::map<std::string, RenderPassInputDeclaration>& GetInputDeclarations() const { return _inputDeclarations; }
	private:
		memory::Ref<Shader> _shader;
		vk::DescriptorPool _descriptorPool;
		std::vector<std::vector<vk::DescriptorSet>> _descriptorSets;
		std::map<std::string, RenderPassInputDeclaration> _inputDeclarations;
		std::map<uint32_t, std::map<uint32_t, ResourceBinding>> _storedResources;
		std::map<uint32_t, vk::WriteDescriptorSet> _writes;

		RenderContext& _context;
		const RenderPassInputDeclaration* GetInputDeclaration(std::string_view name);
	};
}