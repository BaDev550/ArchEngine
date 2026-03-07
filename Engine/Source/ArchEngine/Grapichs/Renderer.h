#pragma once
#include "ShaderLibrary.h"
#include "Model.h"
#include "RenderPass.h"
#include "Texture.h"
#include "ArchEngine/Core/Memory.h"

#include <iostream>

namespace ae { class Scene; }
namespace ae::grapichs {
	class Renderer {
	public:
		static inline const uint32_t MaxFramesInFlight = 2;
		
		static void Init();
		static void Destroy();
		static void BeginFrame();
		static void EndFrame();
		static void BeginDefaultRenderPass();
		static void EndDefaultRenderPass();
		static void DrawVertex(vk::CommandBuffer cmd, memory::Ref<Buffer>& vertexBuffer, uint32_t vertexCount);
		static void DrawIndexed(vk::CommandBuffer cmd, memory::Ref<Buffer>& vertexBuffer, memory::Ref<Buffer>& indexBuffer, uint32_t indexCount);
		static void DrawStaticMesh(memory::Ref<RenderPass>& renderPass, vk::CommandBuffer cmd, memory::Ref<MeshSource>& meshSource, memory::Ref<StaticMesh>& staticMesh);
		static void DrawEnityWithStaticMesh(memory::Ref<RenderPass>& renderPass, vk::CommandBuffer cmd, memory::Ref<MeshSource>& meshSource, memory::Ref<StaticMesh>& staticMesh, const glm::mat4& transform);
		static void CopyBuffer(memory::Ref<Buffer>& src, memory::Ref<Buffer>& dst, vk::DeviceSize size);

		static memory::Ref<Texture2D>& GetWhiteTexture();
		static vk::DescriptorSet GetFinalImageOfScene(memory::Ref<Scene>& scene);
		static vk::CommandBuffer GetCurrentCommandBuffer();
		static uint32_t GetDrawCallCount();
		static uint32_t GetFrameIndex();
		static ShaderLibrary& GetShaderLibrary();
	};
}