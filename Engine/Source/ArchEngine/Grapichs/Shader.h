#pragma once
#include "RenderContext.h"

#include "ArchEngine/Utilities/ShaderCompiler.h"
#include "ArchEngine/Core/Memory.h"

#include <iostream>
#include <unordered_map>
#include <map>

namespace ae::grapichs {
	class Shader : public memory::RefCounted
	{
	public:
		Shader(const std::string& vertexPath, const std::string& fragmentPath);
		~Shader();
		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

		vk::ShaderModule GetFragmentModule() const { return _VertexShaderModule; }
		vk::ShaderModule GetVertexModule() const { return _FragmentShaderModule; }

		const CompiledShaderInfo& GetCompiledShaderData() const { return _CompiledData; }
		const std::map<uint32_t, vk::DescriptorSetLayout>& GetDesciptorLayouts() const { return _DescriptorLayouts; }
		const std::map<uint32_t, std::map<uint32_t, DescriptorInfo>>& GetReflectData() const { return _CompiledData.ReflectData; }
	private:
		void CreateShaderModule(const std::vector<char>& code, vk::ShaderModule* shaderModule);

		CompiledShaderInfo _CompiledData;
		vk::ShaderModule _VertexShaderModule;
		vk::ShaderModule _FragmentShaderModule;
		std::map<uint32_t, vk::DescriptorSetLayout> _DescriptorLayouts;

		RenderContext& _Context;
	};
}