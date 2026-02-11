#pragma once
#include "ArchEngine/Core/Memory.h"
#include "DescriptorManager.h"
#include "Pipeline.h"

namespace ae::grapichs {
	class RenderPass {
	public:
		RenderPass(memory::Ref<Pipeline>& pipeline);
		~RenderPass();

		void Begin();
		void End();

		void SetInput(std::string_view name, const memory::Ref<Buffer>& buffer);
		void SetInput(std::string_view name, const memory::Ref<Texture2D>& texture);

		const memory::Ref<Framebuffer>& GetTargetFramebuffer() const { return _pipeline->GetPipelineData().TargetFramebuffer; }
		const memory::Ref<Pipeline>& GetPipeline() const { return _pipeline; }
	private:
		memory::Ref<DescriptorManager> _descriptorManager;
		memory::Ref<Pipeline> _pipeline;
	};
}