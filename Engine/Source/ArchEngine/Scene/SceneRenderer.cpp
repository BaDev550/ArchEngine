#include "ArchPch.h"
#include "SceneRenderer.h"
#include "ArchEngine/Grapichs/Renderer.h"
#include "ArchEngine/Grapichs/DebugRenderer.h"
#include "ArchEngine/Core/Application.h"
#include "ArchEngine/Scene/Scene.h"
#include <backends/imgui_impl_vulkan.h>

#include "ArchEngine/Objects/Entity_Skybox.h"
#include "ArchEngine/Objects/Entity_DirectionalLight.h"

namespace ae {
	using namespace grapichs;
	SceneRenderer::SceneRenderer(Scene* scene) : _scene(scene) {
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
		}

		// Shadow pass + framebuffer
		{
			FramebufferSpecification framebufferSpecs{};
			framebufferSpecs.Attachments = { vk::Format::eD32Sfloat };
			framebufferSpecs.Width = _directionalLightShadowMap.ShadowMapResolution;
			framebufferSpecs.Height = _directionalLightShadowMap.ShadowMapResolution;
			framebufferSpecs.ClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			framebufferSpecs.DepthClearValue = 1.0f;
			_directionalLightShadowMap.ShadowFramebuffer = memory::Ref<Framebuffer>::Create(framebufferSpecs);

			PipelineData pipelineData{};
			pipelineData.Shader = Renderer::GetShaderLibrary().GetShader("ShadowShader");
			pipelineData.RenderData.CullingEnable = true;
			pipelineData.RenderData.DepthTestEnable = true;
			pipelineData.RenderData.CullMode = vk::CullModeFlagBits::eFront;
			pipelineData.TargetFramebuffer = _directionalLightShadowMap.ShadowFramebuffer;
			_directionalLightShadowMap.ShadowPipeline = memory::Ref<Pipeline>::Create(pipelineData);
			_directionalLightShadowMap.ShadowRenderPass = memory::Ref<RenderPass>::Create(_directionalLightShadowMap.ShadowPipeline);
		}

		// Skybox pipeline
		{
			PipelineData skyboxPipelineData{};
			skyboxPipelineData.Shader = Renderer::GetShaderLibrary().GetShader("SkyboxShader");
			skyboxPipelineData.TargetFramebuffer = _sceneFramebuffer;
			skyboxPipelineData.RenderData.DepthTestEnable = false;
			skyboxPipelineData.RenderData.CullingEnable = false;
			_sceneData.SceneLightEnviromentData.SkyboxPipeline = memory::Ref<grapichs::Pipeline>::Create(skyboxPipelineData);
			_sceneData.SceneLightEnviromentData.EnviromentMap = Renderer::GetBlackEnviroment();
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
			_sceneRenderPass->SetInput("uSkyboxTexture", _sceneData.SceneLightEnviromentData.EnviromentMap->GetEnvironmentMap());
			_sceneRenderPass->SetInput("uShadowMapTexture", _directionalLightShadowMap.ShadowFramebuffer->GetDepthTexture());
			_directionalLightShadowMap.ShadowRenderPass->SetInput("uCamera", _cameraBuffer);

			_pointLightsBuffer = memory::Ref<Buffer>::Create(
				sizeof(UniformBufferPointLights),
				vk::BufferUsageFlagBits::eUniformBuffer,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
			);
			_pointLightsBuffer->Map();
			_directionalLightBuffer = memory::Ref<Buffer>::Create(
				sizeof(UniformBufferDirectionalLight),
				vk::BufferUsageFlagBits::eUniformBuffer,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
			);
			_directionalLightBuffer->Map();
			_sceneRenderPass->SetInput("uPointLights", _pointLightsBuffer);
			_sceneRenderPass->SetInput("uDirectionalLight", _directionalLightBuffer);
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

	void SceneRenderer::RemoveAllDrawnables() {
		_drawnables.clear();
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
		vk::CommandBuffer cmd = grapichs::Renderer::GetCurrentCommandBuffer();

		CollectSceneLightEnviromentData();

		auto& pointLightsUniformData = _sceneData.SceneLightUniformData.UniformPointLights;
		pointLightsUniformData.Count = _sceneData.SceneLightEnviromentData.GetPointLightCount();
		if (pointLightsUniformData.Count > 0) {
			std::memcpy(pointLightsUniformData.PointLights, _sceneData.SceneLightEnviromentData.PointLights.data(), sizeof(PointLight) * pointLightsUniformData.Count);
			_pointLightsBuffer->Write(&pointLightsUniformData);
		}
		_directionalLightBuffer->Write(&_sceneData.SceneLightEnviromentData.DirectionalLight);

		float near_plane = 0.1f, far_plane = 70.0f;
		float lightDistance = 50.0f;
		glm::vec3 lightDir = _sceneData.SceneLightEnviromentData.DirectionalLight.Direction;
		glm::vec3 lightPos =  lightDir * lightDistance;
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

		_sceneData.ActiveCameraData.View = cam->GetView();
		_sceneData.ActiveCameraData.Projection = cam->GetProjection();
		_sceneData.ActiveCameraData.Position = cam->GetPosition();
		_sceneData.ActiveCameraData.LightSpace = lightProjection * lightView;
		_cameraBuffer->Write(&_sceneData.ActiveCameraData);
		_sceneRenderPass->SetInput("uSkyboxTexture", _sceneData.SceneLightEnviromentData.EnviromentMap->GetEnvironmentMap());

		_directionalLightShadowMap.ShadowRenderPass->Begin();
		DrawEntities(cmd, _directionalLightShadowMap.ShadowRenderPass, entities, true);
		_directionalLightShadowMap.ShadowRenderPass->End();

		_sceneRenderPass->Begin();

		cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, _sceneData.SceneLightEnviromentData.SkyboxPipeline->GetPipeline());
		Renderer::DrawVertex(cmd, nullptr, 36);

		DrawEntities(cmd, _sceneRenderPass, entities);

		if (_sceneData.DrawDebugShapes)
			DrawDebugScene(cmd);
		grapichs::debug::DebugRenderer::Update(Application::Get()->GetDeltaTime());
		_sceneRenderPass->End();
	}

	void SceneRenderer::DrawEntities(vk::CommandBuffer cmd, memory::Ref<grapichs::RenderPass>& pass, const std::unordered_map<EntityID, memory::Ref<Entity>>& entities, bool shadowPass) {
		cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, pass->GetPipeline()->GetPipeline());
		for (auto& drawable : _drawnables) {
			if (!drawable.IsVisible)
				continue;

			auto entityIt = entities.find(drawable.OwnerID);
			if (entityIt != entities.end()) {
				const auto& entity = entityIt->second;
				memory::Ref<grapichs::StaticMesh> staticMesh = AssetManager::GetAsset<grapichs::StaticMesh>(drawable.StaticMeshHandle);
				memory::Ref<grapichs::MeshSource> meshSource = AssetManager::GetAsset<grapichs::MeshSource>(staticMesh->GetMeshSource());
				if (shadowPass && drawable.CastShadow) {
					grapichs::Renderer::DrawStaticMeshEntity(pass, cmd, meshSource, staticMesh, entity->GetTransformMatrix());
				}
				else if (!shadowPass) {
					grapichs::Renderer::DrawStaticMeshEntityWithMaterial(pass, cmd, meshSource, staticMesh, entity->GetTransformMatrix());
				}
			}
		}
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
	}

	void SceneRenderer::CollectSceneLightEnviromentData() {
		{
			auto skyBoxes = _scene->Group<Entity_Skybox>();
			for (EntityID id : skyBoxes) {
				if (Entity_Skybox* entity = dynamic_cast<Entity_Skybox*>(_scene->GetEntity(id))) {
					memory::Ref<grapichs::Enviroment> map = AssetManager::GetAsset<grapichs::Enviroment>(entity->GetEnviromentMapHandle());
					if (map) {
						_sceneData.SceneLightEnviromentData.EnviromentMap = map;
						break;
					}
				}
			}
		}
		{
			auto directionalLights = _scene->Group<Entity_DirectionalLight>();
			for (EntityID id : directionalLights) {
				if (auto* entity = dynamic_cast<Entity_DirectionalLight*>(_scene->GetEntity(id))) {
					auto& lightHandle = entity->GetHandle();
					lightHandle.Direction = glm::normalize(entity->GetTransform().GetEulerRotation());
					_sceneData.SceneLightEnviromentData.DirectionalLight = lightHandle;
					break;
				}
			}
		}
	}
}