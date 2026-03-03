#pragma once
#include "Renderer.h"
#include "RenderContext.h"

#include "RenderPass.h"
#include "Model.h"

namespace ae::grapichs {
	class RenderAPI {
	public:
		RenderAPI();
		~RenderAPI();
		void BeginFrame();
		void EndFrame();
		void DrawVertex(vk::CommandBuffer cmd, memory::Ref<Buffer>& vertexBuffer, uint32_t vertexCount);
		void DrawIndexed(vk::CommandBuffer cmd, memory::Ref<Buffer>& vertexBuffer, memory::Ref<Buffer>& indexBuffer, uint32_t indexCount);
		void DrawStaticMesh(memory::Ref<RenderPass>& renderPass, vk::CommandBuffer cmd, memory::Ref<Model>& model);
		vk::CommandBuffer GetCurrentCommandBuffer();
	private:
		struct RenderStats {
			uint32_t DrawCalls;
		} _renderStats;

		struct FrameContext {
			vk::CommandPool CommandPool;
			vk::CommandBuffer CommandBuffer;
		} _frames[Renderer::MaxFramesInFlight];
		bool _frameStarted = false;
		RenderContext& _context;
	};
}