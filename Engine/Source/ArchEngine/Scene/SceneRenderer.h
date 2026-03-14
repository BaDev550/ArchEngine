#pragma once
#include "ArchEngine/Objects/Entity.h"
#include "ArchEngine/Grapichs/Model.h"
#include "ArchEngine/Core/Memory.h"
#include "ArchEngine/Grapichs/Buffer.h"
#include "ArchEngine/Grapichs/Camera.h"
#include "ArchEngine/Grapichs/RenderPass.h"
#include "ArchEngine/Grapichs/Framebuffer.h"
#include "ArchEngine/Grapichs/TextureCube.h"
#include "ArchEngine/Grapichs/Enviroment.h"
#include "ArchEngine/Grapichs/Light.h"
#include "ArchEngine/AssetManager/AssetManager.h"

namespace ae {
	struct Drawnable {
		EntityID OwnerID = 0;
		AssetHandle StaticMeshHandle = INVALID_ASSET_HANDLE;
		bool IsVisible = true;
		bool CastShadow = true;

		Drawnable(EntityID ownerID) : OwnerID(ownerID) {}
		void ImportStaticMesh(const std::string& modelPath) {
			std::filesystem::path staticMeshPath = modelPath;
			staticMeshPath.replace_extension(".mesh");
			AssetHandle sourceMeshHandle = AssetManager::ImportAsset(modelPath);
			const auto& staticMesh = AssetManager::Create<grapichs::StaticMesh>(staticMeshPath.string(), sourceMeshHandle);
			StaticMeshHandle = staticMesh->GetAssetHandle();
		}
	};

	struct CameraData {
		glm::mat4 View;
		glm::mat4 Projection;
		glm::mat4 LightSpace;
		glm::vec3 Position;
	};

	struct LightEnviromentUniformData {
		grapichs::UniformBufferDirectionalLight UniformDirectionalLight;
		grapichs::UniformBufferPointLights UniformPointLights;
	};

	struct LightEnviromentData {
		grapichs::DirectionalLight DirectionalLight;
		std::vector<grapichs::PointLight> PointLights;

		memory::Ref<grapichs::Enviroment> EnviromentMap = nullptr;
		memory::Ref<grapichs::Pipeline> SkyboxPipeline = nullptr;

		uint32_t GetPointLightCount() const { return static_cast<uint32_t>(PointLights.size()); }
	};

	struct SceneData {
		CameraData ActiveCameraData;
		LightEnviromentData SceneLightEnviromentData;
		LightEnviromentUniformData SceneLightUniformData;
		bool DrawDebugShapes = false;
	};

	class Scene;
	class SceneRenderer {
	public:
		SceneRenderer(Scene* scene);
		SceneData& GetSceneData() { return _sceneData; }
		RenderHandle AddDrawnable(EntityID entityID);
		Drawnable& GetDrawnable(const RenderHandle& handle);
		size_t GetDrawnableCount() const { return _drawnables.size(); }
		VkDescriptorSet GetSceneOutputTexture() const { return _sceneFramebuffer->GetAttachmentTexture(0)->GetImGuiTexture(); }
		memory::Ref<grapichs::Framebuffer>& GetDirectionalLightShadowMapFramebuffer() { return _directionalLightShadowMap.ShadowFramebuffer; }
		void RemoveAllDrawnables();
		void RenderScene(const memory::Ref<grapichs::Camera>& cam, const std::unordered_map<EntityID, memory::Ref<Entity>>& entities);
	private:
		std::vector<Drawnable> _drawnables;
		SceneData _sceneData;

		memory::Ref<grapichs::Pipeline> _scenePipeline = nullptr;
		memory::Ref<grapichs::RenderPass> _sceneRenderPass = nullptr;
		memory::Ref<grapichs::Framebuffer> _sceneFramebuffer = nullptr;

		struct DirectionalLightShadow {
			memory::Ref<grapichs::Pipeline> ShadowPipeline = nullptr;
			memory::Ref<grapichs::RenderPass> ShadowRenderPass = nullptr;
			memory::Ref<grapichs::Framebuffer> ShadowFramebuffer = nullptr;
			const uint32_t ShadowMapResolution = 2048;
		} _directionalLightShadowMap;

		memory::Ref<grapichs::Buffer> _cameraBuffer = nullptr;
		memory::Ref<grapichs::Buffer> _pointLightsBuffer = nullptr;
		memory::Ref<grapichs::Buffer> _directionalLightBuffer = nullptr;

		struct DebugDrawData {
			memory::Ref<grapichs::Buffer> LineVertexBuffer;
			memory::Ref<grapichs::Buffer> TriangleVertexBuffer;
			memory::Ref<grapichs::Pipeline> DebugPipeline;
			const uint32_t MAX_LINES = 10000;
			const uint32_t MAX_TRIANGLES = 10000;
		} _debugDrawData;
		void DrawEntities(vk::CommandBuffer cmd, memory::Ref<grapichs::RenderPass>& pass, const std::unordered_map<EntityID, memory::Ref<Entity>>& entities, bool shadowPass = false);
		void DrawDebugScene(vk::CommandBuffer cmd);
		void CollectSceneLightEnviromentData();

		Scene* _scene = nullptr;
	};
}