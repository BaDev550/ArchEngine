#pragma once
#include "Renderer.h"
#include "RenderContext.h"

namespace ae::grapichs {
	class RenderAPI {
	public:
		RenderAPI();
		~RenderAPI();
		void BeginFrame();
		void EndFrame();
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