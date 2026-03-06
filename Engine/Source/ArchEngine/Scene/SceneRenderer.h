#pragma once
#include "ArchEngine/Objects/Entity.h"
#include "ArchEngine/Grapichs/Model.h"
#include "ArchEngine/Core/Memory.h"
#include "ArchEngine/Grapichs/Camera.h"
#include "ArchEngine/Grapichs/RenderPass.h"
#include "ArchEngine/Grapichs/Framebuffer.h"

namespace ae {
	struct Drawnable {
		EntityID OwnerID = 0;
		bool IsVisible = true;
		memory::Ref<grapichs::Model> Model = nullptr;

		Drawnable(EntityID ownerID, const std::string& modelPath) : OwnerID(ownerID) {
			Model = memory::Ref<grapichs::Model>::Create(modelPath);
		}
	};

	struct CameraData {
		glm::mat4 View;
		glm::mat4 Projection;
	};

	struct SceneData {
		CameraData ActiveCameraData;
	};

	class SceneRenderer {
	public:
		SceneRenderer();
		RenderHandle AddDrawnable(EntityID entityID, const std::string& modelPath);
		Drawnable& GetDrawnable(const RenderHandle& handle);
		size_t GetDrawnableCount() const { return _drawnables.size(); }
		VkDescriptorSet GetSceneOutputTexture() const { return _sceneOutTextureID; }
		void RenderScene(const memory::Ref<grapichs::Camera>& cam, const std::unordered_map<EntityID, memory::Ref<Entity>>& entities);
	private:
		std::vector<Drawnable> _drawnables;
		SceneData _sceneData;

		VkDescriptorSet _sceneOutTextureID = nullptr;
		memory::Ref<grapichs::Pipeline> _scenePipeline = nullptr;
		memory::Ref<grapichs::RenderPass> _sceneRenderPass = nullptr;
		memory::Ref<grapichs::Framebuffer> _sceneFramebuffer = nullptr;

		memory::Ref<grapichs::Buffer> _cameraBuffer = nullptr;
	};
}