#include "ArchPch.h"
#include "DescriptorManager.h"

#include "ArchEngine/Core/Application.h"

namespace ae::grapichs {
	DescriptorManager::DescriptorManager(memory::Ref<Shader>& shader)
		: _context(Application::Get()->GetWindow().GetRenderContext())
	{
	}

	DescriptorManager::~DescriptorManager() {
		for (int i = 0; i < Renderer::MaxFramesInFlight; i++)
			_context.GetDevice().freeDescriptorSets(_descriptorPool, static_cast<uint32_t>(_descriptorSets[i].size()), _descriptorSets[i].data());
		_descriptorSets.clear();
		_inputDeclarations.clear();
		_storedResources.clear();
		_context.GetDevice().destroyDescriptorPool(_descriptorPool);
	}

	void DescriptorManager::WriteInput(std::string_view name, memory::Ref<Buffer> buffer) {
		const RenderPassInputDeclaration* decl = GetInputDeclaration(name);
		if (decl)
			_storedResources[decl->Set][decl->Binding].Set(buffer);
	}

	void DescriptorManager::WriteInput(std::string_view name, memory::Ref<Texture2D> texture) {
		const RenderPassInputDeclaration* decl = GetInputDeclaration(name);
		if (decl)
			_storedResources[decl->Set][decl->Binding].Set(texture);
	}

	void DescriptorManager::UpdateSets(vk::CommandBuffer cmd, vk::PipelineLayout layout) {
		uint32_t frameIndex = Renderer::GetFrameIndex();

		for (auto& [setIndex, bindings] : _storedResources) {
			_writes.clear();

			for (auto& [bindingIndex, resource] : bindings) {
				auto& currentData = resource._data[frameIndex];
				if (!currentData) continue;

				switch (resource._type)
				{
				case ShaderReflectionDataType::UniformBuffer: {
					vk::WriteDescriptorSet bufferWrite{};
					bufferWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
					bufferWrite.dstBinding = bindingIndex;
					bufferWrite.dstSet = _descriptorSets[frameIndex][setIndex];
					bufferWrite.pBufferInfo = &currentData.As<Buffer>()->GetDescriptorInfo();
					bufferWrite.descriptorCount = 1;
					_writes[setIndex] = bufferWrite;
					break;
				}
				case ShaderReflectionDataType::Sampler2D: {
					vk::WriteDescriptorSet bufferWrite{};
					bufferWrite.descriptorType = vk::DescriptorType::eSampledImage;
					bufferWrite.dstBinding = bindingIndex;
					bufferWrite.dstSet = _descriptorSets[frameIndex][setIndex];
					bufferWrite.pImageInfo = &currentData.As<Texture2D>()->GetImageDescriptorInfo();
					bufferWrite.descriptorCount = 1;
					_writes[setIndex] = bufferWrite;
					break;
				}
				}
			}

			cmd.bindDescriptorSets(
				vk::PipelineBindPoint::eGraphics,
				layout,
				setIndex,
				1,
				&_descriptorSets[frameIndex][setIndex],
				0, nullptr);
			_context.GetDevice().updateDescriptorSets(1, &_writes[setIndex], 0, nullptr);
		}
	}

	void DescriptorManager::Invalidate() {
		std::vector<vk::DescriptorPoolSize> poolSize;
		poolSize.push_back({ vk::DescriptorType::eUniformBuffer,        1000 });
		poolSize.push_back({ vk::DescriptorType::eStorageImage,         1000 });
		poolSize.push_back({ vk::DescriptorType::eCombinedImageSampler, 1000 });
		poolSize.push_back({ vk::DescriptorType::eSampledImage,         1000 });
		poolSize.push_back({ vk::DescriptorType::eUniformTexelBuffer,   1000 });
		poolSize.push_back({ vk::DescriptorType::eStorageTexelBuffer,   1000 });
		poolSize.push_back({ vk::DescriptorType::eUniformBufferDynamic, 1000 });
		poolSize.push_back({ vk::DescriptorType::eStorageBufferDynamic, 1000 });
		poolSize.push_back({ vk::DescriptorType::eInputAttachment,      1000 });

		vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo{};
		descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSize.size());
		descriptorPoolCreateInfo.pPoolSizes = poolSize.data();
		descriptorPoolCreateInfo.maxSets = 1000;
		descriptorPoolCreateInfo.flags = vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind | vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
		_descriptorPool = _context.GetDevice().createDescriptorPool(descriptorPoolCreateInfo);

		auto& reflectData = _shader->GetReflectData();
		uint32_t maxSet = 0;
		for (const auto& [set, bindings] : reflectData)
			if (set > maxSet) maxSet = set;

		_descriptorSets.resize(Renderer::MaxFramesInFlight);
		for (uint32_t i = 0; i < Renderer::MaxFramesInFlight; i++)
			_descriptorSets[i].resize(maxSet + 1);

		for (const auto& [set, bindings] : reflectData) {
			auto& layout = _shader->GetDescriptorLayout(set);
			for (const auto& [binding, input] : bindings) {
				RenderPassInputDeclaration decl{};
				decl.Set = set;
				decl.Binding = binding;
				decl.Name = input.Name;
				decl.Type = input.Type;
				_inputDeclarations[input.Name] = decl;
			}
			for (uint32_t i = 0; i < Renderer::MaxFramesInFlight; i++)
				_descriptorSets[i][set] = Allocate(layout);
		}
	}

	vk::DescriptorSet DescriptorManager::Allocate(vk::DescriptorSetLayout layout) {
		vk::DescriptorSet set;
		vk::DescriptorSetAllocateInfo allocInfo{};
		allocInfo.descriptorPool = _descriptorPool;
		allocInfo.pSetLayouts = &layout;
		allocInfo.descriptorSetCount = 1;
		_context.GetDevice().allocateDescriptorSets(allocInfo, &set);
		return set;
	}

	const RenderPassInputDeclaration* DescriptorManager::GetInputDeclaration(std::string_view name)
	{
		std::string nameStr(name);
		if (_inputDeclarations.find(nameStr) == _inputDeclarations.end())
			return nullptr;
		const RenderPassInputDeclaration& decl = _inputDeclarations.at(nameStr);
		return &decl;
	}
}