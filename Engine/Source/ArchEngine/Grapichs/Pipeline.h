#pragma once
#include "ArchEngine/Core/Memory.h"

#include "RenderContext.h"
#include "Shader.h"
#include "Framebuffer.h"

namespace ae::grapichs {
	struct PipelineRenderData {
		vk::PrimitiveTopology Topology = vk::PrimitiveTopology::eTriangleList;
		vk::CullModeFlags CullMode = vk::CullModeFlagBits::eBack;
		bool DepthTestEnable = true;
		bool CullingEnable = true;
	};

	struct PipelineData {
		memory::Ref<Shader> Shader;
		memory::Ref<Framebuffer> TargetFramebuffer;
		PipelineRenderData RenderData; // TODO - look into this mybe consider moving the config into constructer
	};

	class Pipeline : public memory::RefCounted {
	public:
		Pipeline(const PipelineData& data);
		~Pipeline();
		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;

		vk::Pipeline GetPipeline() const { return _pipeline; }
		vk::PipelineLayout GetPipelineLayout() const { return _pipelineLayout; }
		const PipelineData& GetPipelineData() const { return _data; }

		void Invalidate();
		const memory::Ref<Shader>& GetShader() const { return _data.Shader; }
	private:
		vk::Pipeline _pipeline;
		vk::PipelineCache _pipelineCache;
		vk::PipelineLayout _pipelineLayout;
		PipelineData _data;

		RenderContext& _context;
	};
}