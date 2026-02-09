#include "ArchPch.h"
#include "Shader.h"
#include "ArchEngine/Core/Application.h"

namespace ae::grapichs {
	Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
		: _Context(Application::Get()->GetWindow().GetRenderContext())
	{
		PROFILE_SCOPE("Compiling shader " + vertexPath);
		std::vector<char> vertShaderCode = ShaderCompiler::CompileShaderFileToSpirv(vertexPath, _CompiledData);
		std::vector<char> fragShaderCode = ShaderCompiler::CompileShaderFileToSpirv(fragmentPath, _CompiledData);
		CreateShaderModule(vertShaderCode, _VertexShaderModule);
		CreateShaderModule(fragShaderCode, _FragmentShaderModule);

		for (auto const& [set, bindings] : _CompiledData.ReflectData)
		{
			std::vector<vk::DescriptorSetLayoutBinding> layoutBindings;
			for (auto const& [binding, info] : bindings) {
				vk::DescriptorType vkType = ShaderReflectionTypeToVulkanType(info.Type);
				vk::DescriptorSetLayoutBinding layoutBinding{
					.binding = binding,
					.descriptorType = vkType,
					.descriptorCount = info.Count,
					.stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment
				};
				layoutBindings.push_back(layoutBinding);
			}
			vk::DescriptorSetLayoutCreateInfo layoutCreateInfo{
				.bindingCount = static_cast<uint32_t>(layoutBindings.size()),
				.pBindings = layoutBindings.data()
			};
			_DescriptorLayouts[set] = _Context.GetDevice().createDescriptorSetLayout(layoutCreateInfo);
		}
	}

	Shader::~Shader()
	{
		for (auto& [set, layout] : _DescriptorLayouts)
			_Context.GetDevice().destroyDescriptorSetLayout(layout);

		_DescriptorLayouts.clear();
		_Context.GetDevice().destroyShaderModule(_VertexShaderModule);
		_Context.GetDevice().destroyShaderModule(_FragmentShaderModule);
	}

	void Shader::CreateShaderModule(const std::vector<char>& code, vk::ShaderModule& shaderModule)
	{
		vk::ShaderModuleCreateInfo createInfo{
			.codeSize = code.size(),
			.pCode = reinterpret_cast<const uint32_t*>(code.data())
		};
		shaderModule = _Context.GetDevice().createShaderModule(createInfo);
	}
}