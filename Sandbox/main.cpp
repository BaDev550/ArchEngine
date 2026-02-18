#include <iostream>
#include <memory>
#include <ArchEngine/Core/Application.h>
#include <ArchEngine/Grapichs/Renderer.h>
#include <ArchEngine/Grapichs/Pipeline.h>
#include <ArchEngine/Grapichs/RenderPass.h>
#include <ArchEngine/Core/EntryPoint.h>

using namespace ae;
class SandboxGame : public Application {
public:
	SandboxGame() {}
	~SandboxGame() = default;

	virtual void ApplicationStarted() override {
		PipelineData pipelineData{};
		pipelineData.Shader = Renderer::GetShaderLibrary().GetShader("ForwardShader");
		pipelineData.TargetFramebuffer = GetWindow().GetDefaultSwapchainFramebuffer();
		_defaultPipeline = memory::Ref<Pipeline>::Create(pipelineData);
		_defaultRenderPass = memory::Ref<RenderPass>::Create(_defaultPipeline);
	}

	virtual void ApplicationUpdate() override {
		Renderer::BeginFrame();
		_defaultRenderPass->Begin();

		vk::CommandBuffer cmd = Renderer::GetCurrentCommandBuffer();

		glm::mat4 transform = glm::mat4(1.0f);
		vk::PipelineLayout layout = _defaultPipeline->GetPipelineLayout();

		cmd.pushConstants(layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4), &transform);
		vkCmdDraw(cmd, 3, 1, 0, 0);

		_defaultRenderPass->End();
		Renderer::EndFrame();
	}
private:
	memory::Ref<grapichs::RenderPass> _defaultRenderPass = nullptr;
	memory::Ref<grapichs::Pipeline> _defaultPipeline = nullptr;
};

ae::Application* CreateApplication() {
	return new SandboxGame();
}