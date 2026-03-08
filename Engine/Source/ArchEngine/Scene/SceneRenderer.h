#pragma once
#include "ArchEngine/Objects/Entity.h"
#include "ArchEngine/Grapichs/Model.h"
#include "ArchEngine/Core/Memory.h"
#include "ArchEngine/Grapichs/Buffer.h"
#include "ArchEngine/Grapichs/Camera.h"
#include "ArchEngine/Grapichs/RenderPass.h"
#include "ArchEngine/Grapichs/Framebuffer.h"
#include "ArchEngine/Grapichs/TextureCube.h"
#include "ArchEngine/AssetManager/AssetManager.h"

namespace ae {
	struct Drawnable {
		EntityID OwnerID = 0;
		AssetHandle StaticMeshHandle = INVALID_ASSET_HANDLE;
		bool IsVisible = true;

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
	};

	struct SceneData {
		CameraData ActiveCameraData;
		bool DrawDebugShapes = false;
	};

	class SceneRenderer {
	public:
		SceneRenderer();
		SceneData& GetSceneData() { return _sceneData; }
		RenderHandle AddDrawnable(EntityID entityID);
		Drawnable& GetDrawnable(const RenderHandle& handle);
		size_t GetDrawnableCount() const { return _drawnables.size(); }
		VkDescriptorSet GetSceneOutputTexture() const { return _sceneOutTextureID; }
		void RemoveAllDrawnables();
		void RenderScene(const memory::Ref<grapichs::Camera>& cam, const std::unordered_map<EntityID, memory::Ref<Entity>>& entities);
	private:
		std::vector<Drawnable> _drawnables;
		SceneData _sceneData;

		VkDescriptorSet _sceneOutTextureID = nullptr;
		memory::Ref<grapichs::Pipeline> _scenePipeline = nullptr;
		memory::Ref<grapichs::RenderPass> _sceneRenderPass = nullptr;
		memory::Ref<grapichs::Framebuffer> _sceneFramebuffer = nullptr;

		memory::Ref<grapichs::Buffer> _cameraBuffer = nullptr;

		struct DebugDrawData {
			memory::Ref<grapichs::Buffer> LineVertexBuffer;
			memory::Ref<grapichs::Buffer> TriangleVertexBuffer;
			memory::Ref<grapichs::Pipeline> DebugPipeline;
			const uint32_t MAX_LINES = 10000;
			const uint32_t MAX_TRIANGLES = 10000;
		} _debugDrawData;
		void DrawDebugScene(vk::CommandBuffer cmd);
	};
}