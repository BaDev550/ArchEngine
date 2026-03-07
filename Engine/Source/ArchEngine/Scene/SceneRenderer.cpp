#include "ArchPch.h"
#include "SceneRenderer.h"
#include "ArchEngine/Grapichs/Renderer.h"
#include <backends/imgui_impl_vulkan.h>

namespace ae {
	using namespace grapichs;
	SceneRenderer::SceneRenderer() {
		{
			FramebufferSpecification sceneFramebufferSpecs{};
			sceneFramebufferSpecs.Attachments = { vk::Format::eR16G16B16A16Sfloat, vk::Format::eD32Sfloat };
			sceneFramebufferSpecs.ClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			sceneFramebufferSpecs.DepthClearValue = 1.0f;
			_sceneFramebuffer = memory::Ref<Framebuffer>::Create(sceneFramebufferSpecs);

			PipelineData scenePipelineData{};
			scenePipelineData.Shader = Renderer::GetShaderLibrary().GetShader("ForwardShader");
			scenePipelineData.TargetFramebuffer = _sceneFramebuffer;
			_scenePipeline = memory::Ref<Pipeline>::Create(scenePipelineData);

			_sceneRenderPass = memory::Ref<RenderPass>::Create(_scenePipeline);
			auto colorAttachment = _sceneFramebuffer->GetAttachmentTexture(0);
			_sceneOutTextureID = ImGui_ImplVulkan_AddTexture(
				(VkSampler)colorAttachment->GetSampler(),
				(VkImageView)colorAttachment->GetImageView(),
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			);
		}
		{
			_cameraBuffer = memory::Ref<Buffer>::Create(
				sizeof(CameraData),
				vk::BufferUsageFlagBits::eUniformBuffer,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
			);
			_cameraBuffer->Map();
			_sceneRenderPass->SetInput("uCamera", _cameraBuffer);
		}
	}

	RenderHandle SceneRenderer::AddDrawnable(EntityID entityID) {
		_drawnables.emplace_back(entityID);
		return RenderHandle{ _drawnables.size() - 1 };
	}

	Drawnable& SceneRenderer::GetDrawnable(const RenderHandle& handle) {
		if (handle.IsValid() && handle.index < _drawnables.size()) {
			return _drawnables[handle.index];
		}
		throw std::runtime_error("Invalid RenderHandle");
	}

	void SceneRenderer::RenderScene(const memory::Ref<grapichs::Camera>& cam, const std::unordered_map<EntityID, memory::Ref<Entity>>& entities) {
		_sceneData.ActiveCameraData.View = cam->GetView();
		_sceneData.ActiveCameraData.Projection = cam->GetProjection();
		_cameraBuffer->Write(&_sceneData.ActiveCameraData);

		_sceneRenderPass->Begin();
		vk::CommandBuffer cmd = grapichs::Renderer::GetCurrentCommandBuffer();
		cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, _sceneRenderPass->GetPipeline()->GetPipeline());

		for (auto& drawable : _drawnables) {
			if (!drawable.IsVisible)
				continue;

			auto entityIt = entities.find(drawable.OwnerID);
			if (entityIt != entities.end()) {
				const auto& entity = entityIt->second;
				memory::Ref<grapichs::StaticMesh> staticMesh = AssetManager::GetAsset<grapichs::StaticMesh>(drawable.StaticMeshHandle);
				memory::Ref<grapichs::MeshSource> meshSource = AssetManager::GetAsset<grapichs::MeshSource>(staticMesh->GetMeshSource());
				grapichs::Renderer::DrawEnityWithStaticMesh(_sceneRenderPass, cmd, meshSource, staticMesh, entity->GetTransformMatrix());
			}
		}
		_sceneRenderPass->End();
	}
}