#include "ArchPch.h"
#include "SceneRenderer.h"
#include "ArchEngine/Grapichs/Renderer.h"
#include "ArchEngine/Grapichs/DebugRenderer.h"
#include "ArchEngine/Core/Application.h"
#include <backends/imgui_impl_vulkan.h>

namespace ae {
	using namespace grapichs;
	SceneRenderer::SceneRenderer() {
		// Scene pipeline / renderpass
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

		// Scene buffers
		{
			_cameraBuffer = memory::Ref<Buffer>::Create(
				sizeof(CameraData),
				vk::BufferUsageFlagBits::eUniformBuffer,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
			);
			_cameraBuffer->Map();
			_sceneRenderPass->SetInput("uCamera", _cameraBuffer);
		}

		// Debug data buffers / pipeline
		{
			_debugDrawData.LineVertexBuffer = memory::Ref<Buffer>::Create(
				_debugDrawData.MAX_LINES * 2 * sizeof(grapichs::debug::DebugVertex),
				vk::BufferUsageFlagBits::eVertexBuffer,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
			);
			_debugDrawData.LineVertexBuffer->Map();
			_debugDrawData.TriangleVertexBuffer = memory::Ref<Buffer>::Create(
				_debugDrawData.MAX_TRIANGLES * 3 * sizeof(grapichs::debug::DebugVertex),
				vk::BufferUsageFlagBits::eVertexBuffer,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
			);
			_debugDrawData.TriangleVertexBuffer->Map();

			PipelineData data{};
			data.RenderData.DepthTestEnable = false;
			data.RenderData.Topology = vk::PrimitiveTopology::eLineList;
			data.TargetFramebuffer = _sceneFramebuffer;
			data.Shader = Renderer::GetShaderLibrary().GetShader("DebugShader");
			_debugDrawData.DebugPipeline = memory::Ref<Pipeline>::Create(data);
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

		DrawDebugScene(cmd);
		_sceneRenderPass->End();
	}

	void SceneRenderer::DrawDebugScene(vk::CommandBuffer cmd)
	{
		const auto& lineCommands = grapichs::debug::DebugRenderer::GetLineDrawCommands();
		if (!lineCommands.empty()) {
			std::vector<grapichs::debug::DebugVertex> lineVertices;
			lineVertices.reserve(lineCommands.size() * 2);

			for (const auto& lineCmd : lineCommands) {
				lineVertices.push_back({ lineCmd.Start, lineCmd.Color });
				lineVertices.push_back({ lineCmd.End, lineCmd.Color });
			}

			uint32_t vertexCount = static_cast<uint32_t>(lineVertices.size());
			vk::DeviceSize bufferSize = vertexCount * sizeof(grapichs::debug::DebugVertex);
			_debugDrawData.LineVertexBuffer->Write(lineVertices.data(), bufferSize);

			glm::mat4 viewProj = _sceneData.ActiveCameraData.Projection * _sceneData.ActiveCameraData.View;
			cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, _debugDrawData.DebugPipeline->GetPipeline());
			cmd.pushConstants(
				_debugDrawData.DebugPipeline->GetPipelineLayout(),
				vk::ShaderStageFlagBits::eVertex,
				0, sizeof(glm::mat4), &viewProj
			);
			grapichs::Renderer::DrawVertex(cmd, _debugDrawData.LineVertexBuffer, vertexCount);
		}
		grapichs::debug::DebugRenderer::Update(Application::Get()->GetDeltaTime());
	}
}