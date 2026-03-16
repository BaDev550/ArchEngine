#include "ArchPch.h"
#include "Renderer.h"
#include "RenderAPI.h"
#include "DebugRenderer.h"
#include "ArchEngine/Core/Application.h"
#include "ArchEngine/Scene/Scene.h"

namespace ae::grapichs {
	struct RenderData {
		memory::Scope<ShaderLibrary> ShaderLibrary = nullptr;
		memory::Ref<Texture2D> WhiteTexture = nullptr;
		memory::Ref<Enviroment> BlackEnviroment = nullptr;
		memory::Ref<TextureCube> BlackCubeMap = nullptr;
	} s_data;
	static RenderAPI* g_renderAPI = nullptr;
	static uint32_t g_frameIndex = 0;
	static memory::Ref<grapichs::RenderPass> _defaultRenderPass = nullptr;
	static memory::Ref<grapichs::Pipeline> _defaultPipeline = nullptr;

	void Renderer::Init() {
		s_data.ShaderLibrary = memory::MakeScope<ShaderLibrary>();
		g_renderAPI = new RenderAPI();

		TextureSpecification specs{};
		specs.Width = 1;
		specs.Height = 1;
		specs.Format = vk::Format::eR8G8B8A8Unorm;
		uint32_t whitePixelData = 0xffffffff;
		s_data.WhiteTexture = memory::Ref<Texture2D>::Create(specs, DataBuffer(&whitePixelData, sizeof(uint32_t)));

		uint32_t blackCubemapData = 0xff000000;
		std::vector<Bitmap> blackCubeMapData = {
			{DataBuffer(&blackCubemapData, sizeof(uint32_t))},
			{DataBuffer(&blackCubemapData, sizeof(uint32_t))},
			{DataBuffer(&blackCubemapData, sizeof(uint32_t))}, 
			{DataBuffer(&blackCubemapData, sizeof(uint32_t))}, 
			{DataBuffer(&blackCubemapData, sizeof(uint32_t))}, 
			{DataBuffer(&blackCubemapData, sizeof(uint32_t))}
		};
		s_data.BlackCubeMap = memory::Ref<TextureCube>::Create(specs, blackCubeMapData);
		s_data.BlackEnviroment = memory::Ref<Enviroment>::Create(s_data.BlackCubeMap);

		PROFILE_SCOPE("Renderer");
		Renderer::GetShaderLibrary().AddShader("ForwardShader","Shaders/forward.vert", "Shaders/forward.frag");
		Renderer::GetShaderLibrary().AddShader("DebugShader",  "Shaders/debug.vert",   "Shaders/debug.frag");
		Renderer::GetShaderLibrary().AddShader("SkyboxShader", "Shaders/skybox.vert",  "Shaders/skybox.frag");
		Renderer::GetShaderLibrary().AddShader("ShadowShader", "Shaders/shadow.vert",  "Shaders/shadow.frag");
		
		debug::DebugRenderer::Init();
		{
			PipelineData pipelineData{};
			pipelineData.Shader = Renderer::GetShaderLibrary().GetShader("ForwardShader");
			pipelineData.TargetFramebuffer = Application::Get()->GetWindow().GetDefaultSwapchainFramebuffer();
			_defaultPipeline = memory::Ref<Pipeline>::Create(pipelineData);
			_defaultRenderPass = memory::Ref<RenderPass>::Create(_defaultPipeline);
		}
	}

	void Renderer::Destroy() {
		debug::DebugRenderer::Destroy();
		_defaultRenderPass = nullptr;
		_defaultPipeline = nullptr;
		s_data.ShaderLibrary = nullptr;
		s_data.WhiteTexture = nullptr;
		s_data.BlackEnviroment = nullptr;
		s_data.BlackCubeMap = nullptr;
		delete g_renderAPI;
	}

	void Renderer::BeginFrame() {
		g_renderAPI->BeginFrame();
	}

	void Renderer::EndFrame() {
		g_renderAPI->EndFrame();
		g_frameIndex = (g_frameIndex + 1) % MaxFramesInFlight;
	}

	void Renderer::BeginDefaultRenderPass(){
		_defaultRenderPass->Begin();
	}

	void Renderer::EndDefaultRenderPass() {
		_defaultRenderPass->End();
	}

	void Renderer::DrawVertex(vk::CommandBuffer cmd, memory::Ref<Buffer> vertexBuffer, uint32_t vertexCount) {
		g_renderAPI->DrawVertex(cmd, vertexBuffer, vertexCount);
	}

	void Renderer::DrawIndexed(vk::CommandBuffer cmd, memory::Ref<Buffer>& vertexBuffer, memory::Ref<Buffer>& indexBuffer, uint32_t indexCount) {
		g_renderAPI->DrawIndexed(cmd, vertexBuffer, indexBuffer, indexCount);
	}

	void Renderer::DrawStaticMesh(memory::Ref<RenderPass>& renderPass, vk::CommandBuffer cmd, memory::Ref<MeshSource>& meshSource, memory::Ref<StaticMesh>& staticMesh) {
		g_renderAPI->DrawStaticMesh(renderPass, cmd, meshSource, staticMesh);
	}

	void Renderer::DrawStaticMeshWithMaterial(memory::Ref<RenderPass>& renderPass, vk::CommandBuffer cmd, memory::Ref<MeshSource>& meshSource, memory::Ref<StaticMesh>& staticMesh) {
		g_renderAPI->DrawStaticMeshWithMaterial(renderPass, cmd, meshSource, staticMesh);
	}

	void Renderer::DrawSkeletalMeshWithMaterial(memory::Ref<RenderPass>& renderPass, vk::CommandBuffer cmd, memory::Ref<MeshSource>& meshSource, memory::Ref<SkeletalMesh>& skeletalMesh) {
		g_renderAPI->DrawSkeletalMeshWithMaterial(renderPass, cmd, meshSource, skeletalMesh);
	}

	void Renderer::CopyBuffer(memory::Ref<Buffer>& src, memory::Ref<Buffer>& dst, vk::DeviceSize size) {
		Application::Get()->GetWindow().GetRenderContext().WaitDeviceIdle();
		Application::Get()->GetWindow().GetRenderContext().CopyBuffer(src->GetBuffer(), dst->GetBuffer(), size);
	}

	memory::Ref<Texture2D>& Renderer::GetWhiteTexture() {
		return s_data.WhiteTexture;
	}

	memory::Ref<TextureCube>& Renderer::GetBlackCubeTexture() {
		return s_data.BlackCubeMap;
	}

	memory::Ref<Enviroment>& Renderer::GetBlackEnviroment() {
		return s_data.BlackEnviroment;
	}

	vk::CommandBuffer Renderer::GetCurrentCommandBuffer() {
		return g_renderAPI->GetCurrentCommandBuffer();
	}
	vk::DescriptorSet Renderer::GetFinalImageOfScene(memory::Ref<Scene>& scene) {
		return scene->GetRenderer().GetSceneOutputTexture();
	}

	ShaderLibrary& Renderer::GetShaderLibrary() { return *s_data.ShaderLibrary; }
	uint32_t Renderer::GetDrawCallCount() { return g_renderAPI->GetRenderStats().DrawCalls; }
	uint32_t Renderer::GetFrameIndex() { return g_frameIndex; }
	RenderStats& Renderer::GetRenderStats() { return g_renderAPI->GetRenderStats(); }
}