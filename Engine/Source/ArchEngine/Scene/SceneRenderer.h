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
#include "ArchEngine/Grapichs/Animator.h"
#include "ArchEngine/AssetManager/AssetManager.h"

namespace ae {
	struct Drawnable {
		EntityID OwnerID = 0;
		AssetHandle MeshHandle = INVALID_ASSET_HANDLE;
		grapichs::Animator* AnimatorInstance = nullptr;
		bool IsVisible = true;
		bool IsSkinned = false;
		bool CastShadow = true;

		Drawnable(EntityID ownerID) : OwnerID(ownerID) {}
		void ImportMesh(const std::string& modelPath) {
			AssetHandle sourceMeshHandle = AssetManager::ImportAsset(modelPath);
			const memory::Ref<grapichs::MeshSource>& meshSource = AssetManager::GetAsset<grapichs::MeshSource>(sourceMeshHandle);

			if (meshSource->isSkinned()) {
				std::filesystem::path skeletalMeshPath = modelPath;
				skeletalMeshPath.replace_extension(".skmesh");
				const auto& skeletalMesh = AssetManager::Create<grapichs::SkeletalMesh>(skeletalMeshPath.string(), sourceMeshHandle);
				MeshHandle = skeletalMesh->GetAssetHandle();
				IsSkinned = true;
			}
			else {
				std::filesystem::path staticMeshPath = modelPath;
				staticMeshPath.replace_extension(".mesh");
				const auto& staticMesh = AssetManager::Create<grapichs::StaticMesh>(staticMeshPath.string(), sourceMeshHandle);
				MeshHandle = staticMesh->GetAssetHandle();
				IsSkinned = false;
			}
		}
	};

	struct CameraData {
		glm::mat4 View;
		glm::mat4 Projection;
		glm::vec3 Position;
		float Near;
		float Far;
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
		memory::Ref<grapichs::Framebuffer>& GetDirectionalLightShadowMapFramebuffer() { return _cascadedDirectionalShadowMap.Framebuffer; }
		void RemoveAllDrawnables();
		void RenderScene(const memory::Ref<grapichs::Camera>& cam, const std::unordered_map<EntityID, memory::Ref<Entity>>& entities);
	private:
		std::vector<Drawnable> _drawnables;
		SceneData _sceneData;

		memory::Ref<grapichs::Pipeline> _scenePipeline = nullptr;
		memory::Ref<grapichs::RenderPass> _sceneRenderPass = nullptr;
		memory::Ref<grapichs::Framebuffer> _sceneFramebuffer = nullptr;

		struct CascadedDirectionalShadowMap {
			memory::Ref<grapichs::Pipeline> Pipeline = nullptr;
			memory::Ref<grapichs::RenderPass> RenderPass = nullptr;
			memory::Ref<grapichs::Framebuffer> Framebuffer = nullptr;
			const uint32_t ShadowCascades = 4;
			const uint32_t ShadowMapResolution = 2048;
			const std::vector<float> ShadowCascadeLevels = { 1000.0f / 50.0f, 1000.0f / 25.0f, 1000.0f / 10.0f, 1000.0f / 2.0f };

			std::vector<glm::mat4> GetLightSpaceMatrices(CameraData& cameraData, const glm::vec3& lightDirection);
			glm::mat4 GetLightSpaceMatrix(CameraData& cameraData, const glm::vec3& lightDirection, float near, float far);
			std::vector<glm::vec4> GetFrustumCornersWorldSpace(const glm::mat4& view, const glm::mat4& proj);

			struct UniformBufferCascades {
				glm::mat4 LightSpaceMatrices[4];
				glm::vec4 CascadeSplits;
			} UniformBufferData;
		} _cascadedDirectionalShadowMap;

		memory::Ref<grapichs::Buffer> _cameraBuffer = nullptr;
		memory::Ref<grapichs::Buffer> _cascadeBuffer = nullptr;
		memory::Ref<grapichs::Buffer> _pointLightsBuffer = nullptr;
		memory::Ref<grapichs::Buffer> _directionalLightBuffer = nullptr;

		struct DebugDrawData {
			memory::Ref<grapichs::Buffer> LineVertexBuffer;
			memory::Ref<grapichs::Buffer> TriangleVertexBuffer;
			memory::Ref<grapichs::Pipeline> DebugPipeline;
			const uint32_t MAX_LINES = 10000;
			const uint32_t MAX_TRIANGLES = 10000;
		} _debugDrawData;
		void DrawEntitiesToMainPass(vk::CommandBuffer cmd, const std::unordered_map<EntityID, memory::Ref<Entity>>& entities);
		void DrawEntitiesToShadowPass(vk::CommandBuffer cmd, const std::unordered_map<EntityID, memory::Ref<Entity>>& entities, const glm::mat4& lightSpaceMatrix);
		void DrawDebugScene(vk::CommandBuffer cmd);
		void CollectSceneLightEnviromentData();

		Scene* _scene = nullptr;
	};
}