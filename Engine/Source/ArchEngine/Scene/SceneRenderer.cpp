#include "ArchPch.h"
#include "SceneRenderer.h"
#include "ArchEngine/Grapichs/Renderer.h"
#include "ArchEngine/Grapichs/DebugRenderer.h"
#include "ArchEngine/Core/Application.h"
#include "ArchEngine/Scene/Scene.h"
#include <backends/imgui_impl_vulkan.h>

#include "ArchEngine/Objects/Entity_Skybox.h"
#include "ArchEngine/Objects/Entity_DirectionalLight.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include <glm/glm.hpp>

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
			framebufferSpecs.Width =  _cascadedDirectionalShadowMap.ShadowMapResolution;
			framebufferSpecs.Height = _cascadedDirectionalShadowMap.ShadowMapResolution;
			framebufferSpecs.Layers = _cascadedDirectionalShadowMap.ShadowCascades;
			framebufferSpecs.ClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			framebufferSpecs.DepthClearValue = 1.0f;
			_cascadedDirectionalShadowMap.Framebuffer = memory::Ref<Framebuffer>::Create(framebufferSpecs);

			PipelineData pipelineData{};
			pipelineData.Shader = Renderer::GetShaderLibrary().GetShader("ShadowShader");
			pipelineData.RenderData.CullingEnable = true;
			pipelineData.RenderData.DepthTestEnable = true;
			pipelineData.RenderData.CullMode = vk::CullModeFlagBits::eFront;
			pipelineData.TargetFramebuffer = _cascadedDirectionalShadowMap.Framebuffer;
			_cascadedDirectionalShadowMap.Pipeline = memory::Ref<Pipeline>::Create(pipelineData);
			_cascadedDirectionalShadowMap.RenderPass = memory::Ref<RenderPass>::Create(_cascadedDirectionalShadowMap.Pipeline);
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
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
				true
			);
			_pointLightsBuffer = memory::Ref<Buffer>::Create(
				sizeof(UniformBufferPointLights),
				vk::BufferUsageFlagBits::eUniformBuffer,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
				true
			);
			_directionalLightBuffer = memory::Ref<Buffer>::Create(
				sizeof(UniformBufferDirectionalLight),
				vk::BufferUsageFlagBits::eUniformBuffer,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
				true
			);
			_cascadeBuffer = memory::Ref<Buffer>::Create(
				sizeof(CascadedDirectionalShadowMap::UniformBufferCascades),
				vk::BufferUsageFlagBits::eUniformBuffer,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
				true
			);
			_sceneRenderPass->SetInput("uCamera",		    _cameraBuffer);
			_sceneRenderPass->SetInput("uSkyboxTexture",    _sceneData.SceneLightEnviromentData.EnviromentMap->GetEnvironmentMap());
			_sceneRenderPass->SetInput("uCascadeShadow",    _cascadeBuffer);
			_sceneRenderPass->SetInput("uShadowMapTexture", _cascadedDirectionalShadowMap.Framebuffer->GetDepthTexture());
			_sceneRenderPass->SetInput("uPointLights",		_pointLightsBuffer);
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

		// Light + Camera data set/write
		auto& pointLightsUniformData = _sceneData.SceneLightUniformData.UniformPointLights;
		pointLightsUniformData.Count = _sceneData.SceneLightEnviromentData.GetPointLightCount();
		if (pointLightsUniformData.Count > 0) {
			std::memcpy(pointLightsUniformData.PointLights, _sceneData.SceneLightEnviromentData.PointLights.data(), sizeof(PointLight) * pointLightsUniformData.Count);
			_pointLightsBuffer->Write(&pointLightsUniformData);
		}
		_directionalLightBuffer->Write(&_sceneData.SceneLightEnviromentData.DirectionalLight);
		_sceneData.ActiveCameraData.View = cam->GetView();
		_sceneData.ActiveCameraData.Projection = cam->GetProjection();
		_sceneData.ActiveCameraData.Position = cam->GetPosition();
		_sceneData.ActiveCameraData.Near = 0.1f;
		_sceneData.ActiveCameraData.Far = 1000.0f;
		_cameraBuffer->Write(&_sceneData.ActiveCameraData);
		_sceneRenderPass->SetInput("uSkyboxTexture", _sceneData.SceneLightEnviromentData.EnviromentMap->GetEnvironmentMap());

		// Shadow pass
		std::vector<glm::mat4> lightSpaceMatrices = _cascadedDirectionalShadowMap.GetLightSpaceMatrices(
			_sceneData.ActiveCameraData, 
			_sceneData.SceneLightEnviromentData.DirectionalLight.Direction
		);
		CascadedDirectionalShadowMap::UniformBufferCascades cascadeData{};
		for (int i = 0; i < _cascadedDirectionalShadowMap.ShadowCascades; i++) {
			_cascadedDirectionalShadowMap.RenderPass->Begin(i);
			cascadeData.LightSpaceMatrices[i] = lightSpaceMatrices[i];
			DrawEntitiesToShadowPass(cmd, entities, lightSpaceMatrices[i]);
			_cascadedDirectionalShadowMap.RenderPass->End(i);
		}
		cascadeData.CascadeSplits = glm::vec4(
			_cascadedDirectionalShadowMap.ShadowCascadeLevels[0],
			_cascadedDirectionalShadowMap.ShadowCascadeLevels[1],
			_cascadedDirectionalShadowMap.ShadowCascadeLevels[2],
			_cascadedDirectionalShadowMap.ShadowCascadeLevels[3]
		);
		_cascadeBuffer->Write(&cascadeData);

		// Main pass
		_sceneRenderPass->Begin();
		cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, _sceneData.SceneLightEnviromentData.SkyboxPipeline->GetPipeline());
		Renderer::DrawVertex(cmd, nullptr, 36);
		DrawEntitiesToMainPass(cmd, entities);
		if (_sceneData.DrawDebugShapes)
			DrawDebugScene(cmd);
		grapichs::debug::DebugRenderer::Update(Application::Get()->GetDeltaTime());
		_sceneRenderPass->End();
	}

	void SceneRenderer::DrawEntitiesToMainPass(vk::CommandBuffer cmd, const std::unordered_map<EntityID, memory::Ref<Entity>>& entities) {
		cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, _scenePipeline->GetPipeline());
		for (auto& drawable : _drawnables) {
			if (!drawable.IsVisible)
				continue;

			auto entityIt = entities.find(drawable.OwnerID);
			if (entityIt != entities.end()) {
				const auto& entity = entityIt->second;

				ModelPushConstantData pcData{};
				pcData.Transform = entity->GetTransformMatrix();
				pcData.IsSkinned = drawable.IsSkinned;
				cmd.pushConstants(_scenePipeline->GetPipelineLayout(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(ModelPushConstantData), &pcData);

				if (drawable.IsSkinned) {
					memory::Ref<grapichs::SkeletalMesh> skeletalMesh = AssetManager::GetAsset<grapichs::SkeletalMesh>(drawable.MeshHandle);
					memory::Ref<grapichs::MeshSource> meshSource = AssetManager::GetAsset<grapichs::MeshSource>(skeletalMesh->GetMeshSource());
					if (drawable.AnimatorInstance) {
						drawable.AnimatorInstance->Update(Application::Get()->GetDeltaTime());
						std::vector<glm::mat4> finalBoneMatrices = drawable.AnimatorInstance->GetFinalBoneMatrices();

						auto& skeletonUniformBuffer = skeletalMesh->GetSkeletonUniformBuffer();
						skeletonUniformBuffer->Write(finalBoneMatrices.data(), finalBoneMatrices.size());
						_sceneRenderPass->SetInput("uBones", skeletonUniformBuffer);
					}
					grapichs::Renderer::DrawSkeletalMeshWithMaterial(_sceneRenderPass, cmd, meshSource, skeletalMesh);
				}
				else {
					memory::Ref<grapichs::StaticMesh> staticMesh = AssetManager::GetAsset<grapichs::StaticMesh>(drawable.MeshHandle);
					memory::Ref<grapichs::MeshSource> meshSource = AssetManager::GetAsset<grapichs::MeshSource>(staticMesh->GetMeshSource());
					grapichs::Renderer::DrawStaticMeshWithMaterial(_sceneRenderPass, cmd, meshSource, staticMesh);
				}
			}
		}
	}

	void SceneRenderer::DrawEntitiesToShadowPass(vk::CommandBuffer cmd, const std::unordered_map<EntityID, memory::Ref<Entity>>& entities, const glm::mat4& lightSpaceMatrix)
	{
		cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, _cascadedDirectionalShadowMap.Pipeline->GetPipeline());
		for (auto& drawable : _drawnables) {
			if (!drawable.IsVisible)
				continue;

			auto entityIt = entities.find(drawable.OwnerID);
			if (entityIt != entities.end()) {
				const auto& entity = entityIt->second;

				struct ShadowPassPushConstantData {
					glm::mat4 Transform;
					glm::mat4 LightSpaceMatrix;
				} pcData;
				pcData.Transform = entity->GetTransformMatrix();
				pcData.LightSpaceMatrix = lightSpaceMatrix;

				cmd.pushConstants(_cascadedDirectionalShadowMap.Pipeline->GetPipelineLayout(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(ShadowPassPushConstantData), &pcData);

				if (drawable.IsSkinned) {

				}
				else {
					memory::Ref<grapichs::StaticMesh> staticMesh = AssetManager::GetAsset<grapichs::StaticMesh>(drawable.MeshHandle);
					memory::Ref<grapichs::MeshSource> meshSource = AssetManager::GetAsset<grapichs::MeshSource>(staticMesh->GetMeshSource());
					if (drawable.CastShadow) {
						grapichs::Renderer::DrawStaticMesh(_cascadedDirectionalShadowMap.RenderPass, cmd, meshSource, staticMesh);
					}
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
					lightHandle.Direction = entity->GetTransform().GetEulerRotation();
					_sceneData.SceneLightEnviromentData.DirectionalLight = lightHandle;
					break;
				}
			}
		}
	}

	std::vector<glm::vec4> SceneRenderer::CascadedDirectionalShadowMap::GetFrustumCornersWorldSpace(const glm::mat4& view, const glm::mat4& proj) {
		const auto inv = glm::inverse(proj * view);

		std::vector<glm::vec4> frustumCorners;
		for (unsigned int x = 0; x < 2; ++x) {
			for (unsigned int y = 0; y < 2; ++y) {
				for (unsigned int z = 0; z < 2; ++z) {
					const glm::vec4 pt = inv * glm::vec4(
						2.0f * x - 1.0f,
						2.0f * y - 1.0f,
						2.0f * z - 1.0f,
						1.0f);
					frustumCorners.push_back(pt / pt.w);
				}
			}
		}

		return frustumCorners;
	}

	glm::mat4 SceneRenderer::CascadedDirectionalShadowMap::GetLightSpaceMatrix(CameraData& cameraData, const glm::vec3& lightDirection, float near, float far) {
		if (near >= far)
			std::swap(near, far);

		float aspect = (float)Application::Get()->GetWindow().GetWidth() / (float)Application::Get()->GetWindow().GetHeight();
		glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, near, far);
		auto cornersWS = GetFrustumCornersWorldSpace(cameraData.View, proj);

		glm::vec3 center(0.0f);
		for (auto& c : cornersWS)
			center += glm::vec3(c);
		center /= float(cornersWS.size());

		glm::vec3 lightDir = glm::normalize(lightDirection);
		glm::vec3 lightPos = center - lightDir;
		glm::mat4 lightView = glm::lookAt(lightPos, center, glm::vec3(0.0f, 1.0f, 0.0f));

		float minX = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::lowest();
		float minY = std::numeric_limits<float>::max();
		float maxY = std::numeric_limits<float>::lowest();
		float minZ = std::numeric_limits<float>::max();
		float maxZ = std::numeric_limits<float>::lowest();
		for (auto& c : cornersWS)
		{
			glm::vec4 trf = lightView * c;
			minX = std::min(minX, trf.x);
			maxX = std::max(maxX, trf.x);
			minY = std::min(minY, trf.y);
			maxY = std::max(maxY, trf.y);
			minZ = std::min(minZ, trf.z);
			maxZ = std::max(maxZ, trf.z);
		}

		constexpr float zMult = 10.0f;
		if (minZ < 0) {
			minZ *= zMult;
		}
		else {
			minZ /= zMult;
		}
		if (maxZ < 0) {
			maxZ /= zMult;
		}
		else {
			maxZ *= zMult;
		}

		glm::mat4 lightOrtho = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
		lightOrtho[1][1] *= -1.0f;
		return lightOrtho * lightView;
	}

	std::vector<glm::mat4> SceneRenderer::CascadedDirectionalShadowMap::GetLightSpaceMatrices(CameraData& cameraData, const glm::vec3& lightDirection) {
		std::vector<glm::mat4> result;
		for (size_t i = 0; i < ShadowCascades + 1; ++i) {
			if (i == 0) {
				result.push_back(GetLightSpaceMatrix(cameraData, lightDirection, cameraData.Near, ShadowCascadeLevels[i]));
			}
			else if (i < ShadowCascades) {
				result.push_back(GetLightSpaceMatrix(cameraData, lightDirection, ShadowCascadeLevels[i - 1], ShadowCascadeLevels[i]));
			}
			else {
				result.push_back(GetLightSpaceMatrix(cameraData, lightDirection, ShadowCascadeLevels[i - 1], cameraData.Far));
			}
		}
		return result;
	}
}