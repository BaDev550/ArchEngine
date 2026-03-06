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
		PipelineData pipelineData{};
		pipelineData.Shader = Renderer::GetShaderLibrary().GetShader("ForwardShader");
		pipelineData.TargetFramebuffer = GetWindow().GetDefaultSwapchainFramebuffer();
		_defaultPipeline = memory::Ref<Pipeline>::Create(pipelineData);
		_defaultRenderPass = memory::Ref<RenderPass>::Create(_defaultPipeline);
		_defaultCamera = memory::Ref<FreeCamera>::Create();

		{
			_cameraBuffer = memory::Ref<Buffer>::Create(
				sizeof(CameraData), 
				vk::BufferUsageFlagBits::eUniformBuffer, 
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
			);
			_cameraBuffer->Map();
			_defaultRenderPass->SetInput("uCamera", _cameraBuffer);
		}

		_defaultModel = memory::Ref<Model>::Create("Resources/Models/mario_2/mario_2.obj");
	}

	virtual void ApplicationUpdate() override {
		Renderer::BeginFrame();
		ImGuiRenderer::Begin();

		// Default renderpass
		_defaultRenderPass->Begin();
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

		Renderer::DrawStaticMesh(_defaultRenderPass, cmd, _defaultModel);
		_defaultRenderPass->End();

		if (Input::IsKeyPressed(key::Tab)) {
			_cursorEnabled = !_cursorEnabled;
			_defaultCamera->SetFirstMouse();
			_defaultCamera->SetProccessingMouse(!_cursorEnabled);
			_window->SetCursor(_cursorEnabled);
		}

		ImGui::ShowDemoWindow();

		ImGuiRenderer::End();
		Renderer::EndFrame();
	}
private:
	struct SceneData {
		CameraData cameraData;
	} _sceneData;
	memory::Ref<grapichs::RenderPass> _defaultRenderPass = nullptr;
	memory::Ref<grapichs::Pipeline> _defaultPipeline = nullptr;
	memory::Ref<grapichs::Model> _defaultModel = nullptr;
	memory::Ref<grapichs::FreeCamera> _defaultCamera = nullptr;

	memory::Ref<grapichs::Buffer> _cameraBuffer = nullptr;
	bool _cursorEnabled = false;
};

ae::Application* CreateApplication() {
	return new SandboxGame();
}