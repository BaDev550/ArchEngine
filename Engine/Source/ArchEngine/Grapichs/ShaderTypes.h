#pragma once
#include <iostream>
#include <vulkan/vulkan.hpp>

namespace ae::grapichs {
	enum class ShaderReflectionDataType : uint8_t {
		None = 0,
		Sampler2D,
		UniformBuffer
	};

	enum class ShaderUniformType {
		None = 0,
		Bool,
		Int,
		Float,
		Vec2,
		Vec3,
		Vec4,
		Mat3,
		Mat4
	};

	struct DescriptorInfo {
		std::string Name;
		ShaderReflectionDataType Type;
		uint32_t Count = 1;
	};

	struct CompiledShaderInfo {
		std::map<uint32_t, std::map<uint32_t, DescriptorInfo>> ReflectData;
		std::vector<vk::VertexInputAttributeDescription> AttribDescriptions;
		vk::VertexInputBindingDescription BindingDescription;
	};

	static constexpr std::string ShaderReflectionDataTypeToString(ShaderReflectionDataType& type) {
		switch (type)
		{
		case ShaderReflectionDataType::None: return "None";
		case ShaderReflectionDataType::UniformBuffer: return "UniformBuffer";
		case ShaderReflectionDataType::Sampler2D: return "Sampler2D";
		default: return "Unknow";
		}
	}

	static constexpr vk::DescriptorType ShaderReflectionTypeToVulkanType(const grapichs::ShaderReflectionDataType& type) {
		switch (type)
		{
		case ShaderReflectionDataType::UniformBuffer: return vk::DescriptorType::eUniformBuffer;
		case ShaderReflectionDataType::Sampler2D: return vk::DescriptorType::eCombinedImageSampler;
		default: return vk::DescriptorType::eUniformBuffer;
		}
	}

	static constexpr uint32_t GetFormatSize(vk::Format format) {
		switch (format)
		{
		case vk::Format::eUndefined: return 0;
		case vk::Format::eR16Sfloat: return 2;
		case vk::Format::eR16G16Sfloat: return 4;
		case vk::Format::eR16G16B16Sfloat: return 8;
		case vk::Format::eR16G16B16A16Sfloat: return 16;

		case vk::Format::eR32Sfloat: return 4;
		case vk::Format::eR32G32Sfloat: return 8;
		case vk::Format::eR32G32B32Sfloat: return 12;
		case vk::Format::eR32G32B32A32Sfloat: return 16;
		}
	}
}