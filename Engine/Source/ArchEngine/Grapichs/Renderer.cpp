#include "ArchPch.h"
#include "Renderer.h"

namespace ae::grapichs {
	struct RenderData {
		memory::Scope<ShaderLibrary> ShaderLibrary = nullptr;
	} s_data;

	void Renderer::Init() {
		s_data.ShaderLibrary = memory::MakeScope<ShaderLibrary>();

		PROFILE_SCOPE("Renderer");
		Renderer::GetShaderLibrary().AddShader("ForwardShader", "Shaders/forward.vert", "Shaders/forward.frag");
	}

	void Renderer::Destroy() {
		s_data.ShaderLibrary = nullptr;
	}

	ShaderLibrary& Renderer::GetShaderLibrary() { return *s_data.ShaderLibrary; }
}