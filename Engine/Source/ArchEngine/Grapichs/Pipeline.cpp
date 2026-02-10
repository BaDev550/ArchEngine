#include "ArchPch.h"
#include "Pipeline.h"
#include "ArchEngine/Core/Application.h"

#include <glm/glm.hpp>

namespace ae::grapichs {
	Pipeline::Pipeline(const PipelineData& data) 
		: _context(Application::Get()->GetWindow().GetRenderContext()), _data(data)
	{
		Invalidate();
	}

	void Pipeline::Invalidate() {
		PipelineConfig config{};
		PipelineConfig::Default(config);
		auto& shader = _data.Shader;
		auto& compiledData = shader->GetCompiledShaderData();

		const vk::PipelineVertexInputStateCreateInfo vertexInputInfo{
			.vertexBindingDescriptionCount = 1,
			.pVertexBindingDescriptions = &compiledData.BindingDescription,
			.vertexAttributeDescriptionCount = static_cast<uint32_t>(compiledData.AttribDescriptions.size()),
			.pVertexAttributeDescriptions = compiledData.AttribDescriptions.data()
		};

		const vk::PipelineShaderStageCreateInfo vertShaderStageInfo{
			.stage = vk::ShaderStageFlagBits::eVertex,
			.module = shader->GetVertexModule(),
			.pName = "main",
		};
		const vk::PipelineShaderStageCreateInfo fragShaderStageInfo{
			.stage = vk::ShaderStageFlagBits::eFragment,
			.module = shader->GetFragmentModule(),
			.pName = "main"
		};
		const vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		std::vector<vk::DescriptorSetLayout> layouts;
		const auto& layoutMap = shader->GetDesciptorLayouts();
		for (const auto& [set, layout] : layoutMap)
			layouts.push_back(layout);

		vk::PushConstantRange pushConstant{
			.stageFlags = vk::ShaderStageFlagBits::eVertex,
			.offset = 0,
			.size = sizeof(glm::mat4)
		};

		vk::PipelineLayoutCreateInfo layoutCreateInfo{
			.setLayoutCount = static_cast<uint32_t>(layouts.size()),
			.pSetLayouts = layouts.data(),
			.pushConstantRangeCount = 1,
			.pPushConstantRanges = &pushConstant
		};
		_pipelineLayout = _context.GetDevice().createPipelineLayout(layoutCreateInfo);

		vk::PipelineCacheCreateInfo cacheCreateInfo{
			.initialDataSize = 0,
			.pInitialData = nullptr,
		};
		_pipelineCache = _context.GetDevice().createPipelineCache(cacheCreateInfo);

		const vk::Format colorAttachmentFormat = Application::Get()->GetWindow().GetSwapchain().GetSwapchainFormat();
		const vk::PipelineRenderingCreateInfo pipelineRenderingInfo{
			.colorAttachmentCount = 1,
			.pColorAttachmentFormats = &colorAttachmentFormat, // Change this later to match the framebuffer format
			.depthAttachmentFormat = vk::Format::eD32Sfloat
		};

		const vk::GraphicsPipelineCreateInfo createInfo{
			.pNext = &pipelineRenderingInfo,
			.stageCount = 2,
			.pStages = shaderStages,
			.pVertexInputState = &vertexInputInfo,
			.pInputAssemblyState = &config.InputAssembyCreateInfo,
			.pViewportState =	   &config.ViewportStateCreateInfo,
			.pRasterizationState = &config.ResterizationStateCreateInfo,
			.pMultisampleState   = &config.MultisampleStateCreateInfo,
			.pDepthStencilState  = &config.DepthStencilCreateInfo,
			.pColorBlendState    = &config.ColorBlendStateCreateInfo,
			.pDynamicState       = &config.DynamicStateCreateInfo,
			.layout = _pipelineLayout
		};
		auto result = _context.GetDevice().createGraphicsPipeline(_pipelineCache, createInfo);
		CHECKF(result.result == vk::Result::eSuccess, "Failed to create pipeline");
		_pipeline = result.value;
	}

	Pipeline::~Pipeline() {
		_context.GetDevice().destroyPipelineLayout(_pipelineLayout);
		_context.GetDevice().destroyPipelineCache(_pipelineCache);
		_context.GetDevice().destroyPipeline(_pipeline);
	}
}