#include <iostream>
#include <memory>
#include <ArchEngine/Core/Application.h>
#include <ArchEngine/Grapichs/Renderer.h>
#include <ArchEngine/Grapichs/Pipeline.h>
#include <ArchEngine/Grapichs/RenderPass.h>
#include <ArchEngine/Grapichs/FreeCamera.h>
#include <ArchEngine/Core/EntryPoint.h>
#include <ArchEngine/Core/Input.h>

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

using namespace ae;
class SandboxGame : public Application {
public:
	struct CameraData {
		glm::mat4 View;
		glm::mat4 Projection;
	};

	SandboxGame() {}
	~SandboxGame() = default;

	virtual void ApplicationStarted() override {
		{
			PipelineData pipelineData{};
			pipelineData.Shader = Renderer::GetShaderLibrary().GetShader("ForwardShader");
			pipelineData.TargetFramebuffer = GetWindow().GetDefaultSwapchainFramebuffer();
			_defaultPipeline = memory::Ref<Pipeline>::Create(pipelineData);
			_defaultRenderPass = memory::Ref<RenderPass>::Create(_defaultPipeline);
		}

		{
			FramebufferSpecification offscreenFramebufferSpecs{};
			offscreenFramebufferSpecs.Attachments = { vk::Format::eR16G16B16A16Sfloat, vk::Format::eD32Sfloat };
			offscreenFramebufferSpecs.ClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			offscreenFramebufferSpecs.DepthClearValue = 1.0f;
			_offscreenFramebuffer = memory::Ref<Framebuffer>::Create(offscreenFramebufferSpecs);
			PipelineData offscreenPipelineData{};
			offscreenPipelineData.Shader = Renderer::GetShaderLibrary().GetShader("ForwardShader");
			offscreenPipelineData.TargetFramebuffer = _offscreenFramebuffer;
			_offscreenPipeline = memory::Ref<Pipeline>::Create(offscreenPipelineData);
			_offscreenRenderPass = memory::Ref<RenderPass>::Create(_offscreenPipeline);
			auto colorAttachment = _offscreenFramebuffer->GetAttachmentTexture(0);
			_viewportTextureID = ImGui_ImplVulkan_AddTexture(
				(VkSampler)colorAttachment->GetSampler(),
				(VkImageView)colorAttachment->GetImageView(),
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			);
		}

		_defaultCamera = memory::Ref<FreeCamera>::Create();

		{
			_cameraBuffer = memory::Ref<Buffer>::Create(
				sizeof(CameraData), 
				vk::BufferUsageFlagBits::eUniformBuffer, 
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
			);
			_cameraBuffer->Map();
			_offscreenRenderPass->SetInput("uCamera", _cameraBuffer);
		}

		_defaultModel = memory::Ref<Model>::Create("Resources/Models/mario_2/mario_2.obj");
	}

	virtual void ApplicationUpdate() override {
		Renderer::BeginFrame();
		ImGuiRenderer::Begin();

		_offscreenRenderPass->Begin();
		vk::CommandBuffer cmd = Renderer::GetCurrentCommandBuffer();

		_defaultCamera->Update(_deltaTime);
		_sceneData.cameraData = {
			.View = _defaultCamera->GetView(),
			.Projection = _defaultCamera->GetProjection()
		};
		_cameraBuffer->Write(&_sceneData.cameraData);

		glm::mat4 transform = glm::mat4(1.0f);
		vk::PipelineLayout layout = _defaultPipeline->GetPipelineLayout();
		cmd.pushConstants(layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4), &transform);

		Renderer::DrawStaticMesh(_offscreenRenderPass, cmd, _defaultModel);
		_offscreenRenderPass->End();

		if (Input::IsKeyPressed(key::Tab)) {
			_cursorEnabled = !_cursorEnabled;
			_defaultCamera->SetFirstMouse();
			_defaultCamera->SetProccessingMouse(!_cursorEnabled);
			_window->SetCursor(_cursorEnabled);
		}

		if (Input::IsKeyPressed(key::F1)) {
			_debugOverlayEnabled = !_debugOverlayEnabled;
		}

		_defaultRenderPass->Begin();

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport", nullptr, windowFlags);
		ImGui::PopStyleVar();
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		ImGui::Image((ImTextureID)_viewportTextureID, viewportPanelSize);
		ImGui::End();

		if (_debugOverlayEnabled)
			DrawDebugOverlay();

		_defaultRenderPass->End();

		ImGuiRenderer::End();
		Renderer::EndFrame();
	}

	void DrawDebugOverlay() {
		ImGuiWindowFlags overlayFlags = ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoMove;

		ImGuiViewport* mainViewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos({ mainViewport->WorkPos.x, mainViewport->WorkPos.y }, ImGuiCond_Always, ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize({ mainViewport->WorkSize.x, mainViewport->WorkSize.y / 2 });
		ImGui::SetNextWindowBgAlpha(0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		if (ImGui::Begin("Debug Overlay", nullptr, overlayFlags)) {
			ImGui::Text("ArchEngine Debug Info");
			ImGui::Separator();
			ImGui::Text("Cursor Enabled: %s", _cursorEnabled ? "True" : "False");

			if (ImGui::CollapsingHeader("Render stats")) {
				ImGui::Text("FPS: %.1f", 1.0f / _deltaTime);
				ImGui::Text("Draw Calls: %d", Renderer::GetDrawCallCount());
				ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
			}
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}
private:
	struct SceneData {
		CameraData cameraData;
	} _sceneData;
	memory::Ref<grapichs::RenderPass> _defaultRenderPass = nullptr;
	memory::Ref<grapichs::Pipeline> _defaultPipeline = nullptr;
	memory::Ref<grapichs::Model> _defaultModel = nullptr;
	memory::Ref<grapichs::FreeCamera> _defaultCamera = nullptr;

	memory::Ref<grapichs::RenderPass> _offscreenRenderPass = nullptr;
	memory::Ref<grapichs::Framebuffer> _offscreenFramebuffer = nullptr;
	memory::Ref<grapichs::Pipeline> _offscreenPipeline = nullptr;

	memory::Ref<grapichs::Buffer> _cameraBuffer = nullptr;
	VkDescriptorSet _viewportTextureID = nullptr;
	bool _cursorEnabled = false;
	bool _debugOverlayEnabled = false;
};

ae::Application* CreateApplication() {
	return new SandboxGame();
}