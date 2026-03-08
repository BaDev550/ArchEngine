#pragma once
#include <iostream>
#include <map>
#include <unordered_map>
#include <vulkan/vulkan.hpp>

namespace ae::grapichs {
	enum class ShaderReflectionDataType : uint8_t {
		None = 0,
		Sampler2D,
		SamplerCube,
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

	class ShaderUniform {
	public:
		ShaderUniform() = default;
		ShaderUniform(const std::string& name, ShaderUniformType type, uint32_t size, uint32_t offset)
			: _name(name), _type(type), _size(size), _offset(offset) {
		}
		const std::string& GetName() const { return _name; }
		const uint32_t& GetSize() const { return _size; }
		const uint32_t& GetOffset() const { return _offset; }
	private:
		std::string _name;
		ShaderUniformType _type = ShaderUniformType::None;
		uint32_t _size = 0;
		uint32_t _offset = 0;
	};

	struct ShaderBuffer {
		std::string Name;
		uint32_t Size = 0;
		std::unordered_map<std::string, ShaderUniform> Uniforms;
	};

	struct CompiledShaderInfo {
		std::unordered_map<std::string, ShaderBuffer> ShaderBuffers;
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
		case ShaderReflectionDataType::SamplerCube: return "SamplerCube";
		default: return "Unknow";
		}
	}

	static constexpr vk::DescriptorType ShaderReflectionTypeToVulkanType(const grapichs::ShaderReflectionDataType& type) {
		switch (type)
		{
		case ShaderReflectionDataType::UniformBuffer: return vk::DescriptorType::eUniformBuffer;
		case ShaderReflectionDataType::Sampler2D: return vk::DescriptorType::eCombinedImageSampler;
		case ShaderReflectionDataType::SamplerCube: return vk::DescriptorType::eCombinedImageSampler;
		default: return vk::DescriptorType::eUniformBuffer;
		}
	}

	static constexpr uint32_t GetFormatSize(vk::Format format) {
		switch (format)
		{
		case vk::Format::eUndefined: return 0;
		case vk::Format::eR8G8B8A8Unorm: return 4;
		case vk::Format::eR8G8B8A8Srgb: return 4;
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

	static constexpr vk::ImageAspectFlags TextureFormatToAspectFlags(vk::Format format) {
		switch (format)
		{
		case vk::Format::eUndefined: return vk::ImageAspectFlagBits::eColor;
		case vk::Format::eR8G8B8A8Unorm: return vk::ImageAspectFlagBits::eColor;
		case vk::Format::eR8G8B8A8Srgb: return vk::ImageAspectFlagBits::eColor;
		case vk::Format::eR16Sfloat: return vk::ImageAspectFlagBits::eColor;
		case vk::Format::eR16G16Sfloat: return vk::ImageAspectFlagBits::eColor;
		case vk::Format::eR16G16B16Sfloat: return vk::ImageAspectFlagBits::eColor;
		case vk::Format::eR16G16B16A16Sfloat: return vk::ImageAspectFlagBits::eColor;
		case vk::Format::eR32Sfloat: return vk::ImageAspectFlagBits::eColor;
		case vk::Format::eR32G32Sfloat: return vk::ImageAspectFlagBits::eColor;
		case vk::Format::eR32G32B32Sfloat: return vk::ImageAspectFlagBits::eColor;
		case vk::Format::eR32G32B32A32Sfloat: return vk::ImageAspectFlagBits::eColor;
		case vk::Format::eD32Sfloat: return vk::ImageAspectFlagBits::eDepth;
		case vk::Format::eD24UnormS8Uint: return vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
		}
	}
}