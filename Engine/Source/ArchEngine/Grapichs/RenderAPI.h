#pragma once
#include "Renderer.h"
#include "RenderContext.h"

#include "RenderPass.h"
#include "Model.h"

namespace ae::grapichs {
	struct RenderStats {
		uint32_t DrawCalls;
	};

	class RenderAPI {
	public:

		RenderAPI();
		~RenderAPI();
		void BeginFrame();
		void EndFrame();
		void DrawVertex(vk::CommandBuffer cmd, memory::Ref<Buffer> vertexBuffer, uint32_t vertexCount);
		void DrawIndexed(vk::CommandBuffer cmd, memory::Ref<Buffer>& vertexBuffer, memory::Ref<Buffer>& indexBuffer, uint32_t indexCount);
		void DrawStaticMesh(memory::Ref<RenderPass>& renderPass, vk::CommandBuffer cmd, memory::Ref<MeshSource>& meshSource, memory::Ref<StaticMesh>& staticMesh);
		void DrawStaticMeshEntity(memory::Ref<RenderPass>& renderPass, vk::CommandBuffer cmd, memory::Ref<MeshSource>& meshSource, memory::Ref<StaticMesh>& staticMesh, const glm::mat4& transform);
		void DrawStaticMeshEntityWithMaterial(memory::Ref<RenderPass>& renderPass, vk::CommandBuffer cmd, memory::Ref<MeshSource>& meshSource, memory::Ref<StaticMesh>& staticMesh, const glm::mat4& transform);
		vk::CommandBuffer GetCurrentCommandBuffer();
		RenderStats& GetRenderStats() { return _renderStats; }
	private:
		struct FrameContext {
			vk::CommandPool CommandPool;
			vk::CommandBuffer CommandBuffer;
		} _frames[Renderer::MaxFramesInFlight];
		bool _frameStarted = false;
		RenderStats _renderStats;
		RenderContext& _context;
	};
}