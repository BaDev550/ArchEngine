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
		config.InputAssembyCreateInfo.topology = _data.RenderData.Topology;
		config.DepthStencilCreateInfo.depthTestEnable = _data.RenderData.DepthTestEnable ? vk::True : vk::False;
		config.ResterizationStateCreateInfo.cullMode = _data.RenderData.CullingEnable ? vk::CullModeFlagBits::eBack : vk::CullModeFlagBits::eNone;
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

		std::vector<vk::Format> colorFormats;
		vk::Format depthFormat = vk::Format::eUndefined;
		if (_data.TargetFramebuffer->GetSpecification().IsSwapchain) {
			auto& swapchain = Application::Get()->GetWindow().GetSwapchain();
			colorFormats.push_back(swapchain.GetSwapchainFormat());
			depthFormat = swapchain.GetSwapchainDepthFormat();
		}
		else {
			for (const auto& attachment : _data.TargetFramebuffer->GetSpecification().Attachments.Attachments) {
				if (!IsDepthFormat(attachment)) {
					colorFormats.push_back(attachment);
				}
				else {
					depthFormat = attachment;
				}
			}
		}
		std::vector<vk::PipelineColorBlendAttachmentState> blendAttachments(colorFormats.size());
		for (auto& attachment : blendAttachments) {
			attachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
			attachment.blendEnable = vk::False;
		}
		config.ColorBlendStateCreateInfo.logicOpEnable = vk::False;
		config.ColorBlendStateCreateInfo.logicOp = vk::LogicOp::eCopy;
		config.ColorBlendStateCreateInfo.attachmentCount = static_cast<uint32_t>(blendAttachments.size());
		config.ColorBlendStateCreateInfo.pAttachments = blendAttachments.data();

		const vk::PipelineRenderingCreateInfo pipelineRenderingInfo{
			.colorAttachmentCount = static_cast<uint32_t>(colorFormats.size()),
			.pColorAttachmentFormats = colorFormats.data(),
			.depthAttachmentFormat = depthFormat
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