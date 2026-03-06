#include "ArchPch.h"
#include "Renderer.h"
#include "RenderAPI.h"
#include "ArchEngine/Core/Application.h"

namespace ae::grapichs {
	struct RenderData {
		memory::Scope<ShaderLibrary> ShaderLibrary = nullptr;
		memory::Ref<Texture2D> WhiteTexture = nullptr;
	} s_data;
	static RenderAPI* g_renderAPI = nullptr;
	static uint32_t g_frameIndex = 0;

	void Renderer::Init() {
		s_data.ShaderLibrary = memory::MakeScope<ShaderLibrary>();
		g_renderAPI = new RenderAPI();

		TextureSpecification whiteTextureSpecs{};
		whiteTextureSpecs.Width = 1;
		whiteTextureSpecs.Height = 1;
		uint32_t whitePixelData = 0xffffffff;
		s_data.WhiteTexture = memory::Ref<Texture2D>::Create(whiteTextureSpecs, DataBuffer(&whitePixelData, sizeof(uint32_t)));

		PROFILE_SCOPE("Renderer");
		Renderer::GetShaderLibrary().AddShader("ForwardShader", "Shaders/forward.vert", "Shaders/forward.frag");
	}

	void Renderer::Destroy() {
		s_data.ShaderLibrary = nullptr;
		s_data.WhiteTexture = nullptr;
		delete g_renderAPI;
	}

	void Renderer::BeginFrame() {
		g_renderAPI->BeginFrame();
	}

	void Renderer::EndFrame() {
		g_renderAPI->EndFrame();
		g_frameIndex = (g_frameIndex + 1) % MaxFramesInFlight;
	}

	void Renderer::DrawVertex(vk::CommandBuffer cmd, memory::Ref<Buffer>& vertexBuffer, uint32_t vertexCount) {
		g_renderAPI->DrawVertex(cmd, vertexBuffer, vertexCount);
	}

	void Renderer::DrawIndexed(vk::CommandBuffer cmd, memory::Ref<Buffer>& vertexBuffer, memory::Ref<Buffer>& indexBuffer, uint32_t indexCount) {
		g_renderAPI->DrawIndexed(cmd, vertexBuffer, indexBuffer, indexCount);
	}

	void Renderer::DrawStaticMesh(memory::Ref<RenderPass>& renderPass, vk::CommandBuffer cmd, memory::Ref<Model>& model) {
		g_renderAPI->DrawStaticMesh(renderPass, cmd, model);
	}

	void Renderer::CopyBuffer(memory::Ref<Buffer>& src, memory::Ref<Buffer>& dst, vk::DeviceSize size) {
		Application::Get()->GetWindow().GetRenderContext().WaitDeviceIdle();
		Application::Get()->GetWindow().GetRenderContext().CopyBuffer(src->GetBuffer(), dst->GetBuffer(), size);
	}

	memory::Ref<Texture2D>& Renderer::GetWhiteTexture() {
		return s_data.WhiteTexture;
	}

	vk::CommandBuffer Renderer::GetCurrentCommandBuffer() {
		return g_renderAPI->GetCurrentCommandBuffer();
	}
	ShaderLibrary& Renderer::GetShaderLibrary() { return *s_data.ShaderLibrary; }
	uint32_t Renderer::GetFrameIndex() { return g_frameIndex; }
}