#include "ArchPch.h"
#include "Renderer.h"
#include "RenderAPI.h"

namespace ae::grapichs {
	struct RenderData {
		memory::Scope<ShaderLibrary> ShaderLibrary = nullptr;
	} s_data;
	static RenderAPI* g_renderAPI = nullptr;
	static uint32_t g_frameIndex = 0;

	void Renderer::Init() {
		s_data.ShaderLibrary = memory::MakeScope<ShaderLibrary>();
		g_renderAPI = new RenderAPI();

		PROFILE_SCOPE("Renderer");
		Renderer::GetShaderLibrary().AddShader("ForwardShader", "Shaders/forward.vert", "Shaders/forward.frag");
	}

	void Renderer::Destroy() {
		s_data.ShaderLibrary = nullptr;
		delete g_renderAPI;
	}

	void Renderer::BeginFrame() {
		g_renderAPI->BeginFrame();
	}

	void Renderer::EndFrame() {
		g_renderAPI->EndFrame();
		g_frameIndex = (g_frameIndex + 1) % MaxFramesInFlight;
	}

	vk::CommandBuffer Renderer::GetCurrentCommandBuffer() {
		return g_renderAPI->GetCurrentCommandBuffer();
	}
	ShaderLibrary& Renderer::GetShaderLibrary() { return *s_data.ShaderLibrary; }
	uint32_t Renderer::GetFrameIndex() { return g_frameIndex; }
}