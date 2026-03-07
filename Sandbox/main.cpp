#include <iostream>
#include <memory>
#include <ArchEngine/Core/Application.h>
#include <ArchEngine/Grapichs/Renderer.h>
#include <ArchEngine/Grapichs/FreeCamera.h>
#include <ArchEngine/Core/EntryPoint.h>
#include <ArchEngine/Core/Input.h>
#include <ArchEngine/Scene/Scene.h>
#include <ArchEngine/AssetManager/AssetManager.h>

#include "BasicObject.h"
#include "PhysicsObject.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <backends/imgui_impl_vulkan.h>

using namespace ae;
class SandboxGame : public Application {
public:
	SandboxGame() {}
	~SandboxGame() = default;

	virtual void ApplicationStarted() override {
		_defaultCamera = memory::Ref<FreeCamera>::Create();
		_defaultScene = memory::Ref<Scene>::Create();
		_basicObject = _defaultScene->CreateEntity<BasicObject>();

		_physicsObject = _defaultScene->CreateEntity<PhysicsObject>(ae::PhysicsMotionType::Dynamic, glm::vec3(0.0f, 10.0f, 0.0f));
		_defaultScene->CreateEntity<PhysicsObject>(ae::PhysicsMotionType::Kinematic, glm::vec3(0.0f, 0.0f, 0.0f));
	}

	virtual void ApplicationUpdate() override {
		Renderer::BeginFrame();
		ImGuiRenderer::Begin();
		_defaultCamera->Update(_deltaTime);

		if (Input::IsKeyJustPressed(key::Tab)) {
			_cursorEnabled = !_cursorEnabled;
			_defaultCamera->SetFirstMouse();
			_defaultCamera->SetProccessingMouse(!_cursorEnabled);
			_window->SetCursor(_cursorEnabled);
		}

		if (Input::IsKeyJustPressed(key::F1)) {
			_debugOverlayEnabled = !_debugOverlayEnabled;
		}

		_defaultScene->OnEditorUpdate(_defaultCamera, _deltaTime);

		Renderer::BeginDefaultRenderPass();
		DrawViewport();
		if (_debugOverlayEnabled)
			DrawDebugOverlay();
		Renderer::EndDefaultRenderPass();

		ImGuiRenderer::End();
		Renderer::EndFrame();
	}

	void DrawViewport() {
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport", nullptr, windowFlags);
		ImGui::PopStyleVar();
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		ImGui::Image((ImTextureID)(VkDescriptorSet)Renderer::GetFinalImageOfScene(_defaultScene), viewportPanelSize);
		ImGui::End();
	}

	void DrawDebugOverlay() {
		ImGuiWindowFlags overlayFlags = ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoMove;

		ImGuiViewport* mainViewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos({ mainViewport->WorkPos.x, mainViewport->WorkPos.y }, ImGuiCond_Always, ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize({ mainViewport->WorkSize.x, mainViewport->WorkSize.y / 2 });
		ImGui::SetNextWindowBgAlpha(0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		if (ImGui::Begin("Debug Overlay", nullptr, overlayFlags)) {
			ImGui::Text("ArchEngine Debug Info");
			ImGui::Separator();
			ImGui::Text("Cursor Enabled: %s", _cursorEnabled ? "True" : "False");

			if (ImGui::CollapsingHeader("Render stats")) {
				ImGui::Text("FPS: %.1f", 1.0f / _deltaTime);
				ImGui::Text("Draw Calls: %d", Renderer::GetDrawCallCount());
				ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
			}
			if (ImGui::CollapsingHeader("Scene Hierarchy")) {
				for (auto& [id, entity] : _defaultScene->GetEntities()) {
					std::string entityName = entity->GetName() + "##" + entity->GetID().ToString();
					if (ImGui::Selectable(entityName.c_str(), entity == _selectedEntity)) {
						_selectedEntity = entity.Get();
					}
				}
				if (_selectedEntity) {
					ImGui::Separator();
					ImGui::Text("Entity ID: %d", _selectedEntity->GetID());
					ImGui::DragFloat3("Position", glm::value_ptr(_selectedEntity->GetTransform().Position), 0.1f);
					ImGui::DragFloat3("Rotation", glm::value_ptr(_selectedEntity->GetTransform().Rotation), 0.1f);
					ImGui::DragFloat3("Scale",    glm::value_ptr(_selectedEntity->GetTransform().Scale), 0.1f);
				}
			}
			if (ImGui::CollapsingHeader("Asset Manager")) {
				auto loadedAssets = AssetManager::GetLoadedAssets();
				for (const auto& [assethandle, asset] : loadedAssets) {
					AssetMetadata mtd = AssetManager::GetAssetMetadata(assethandle);
					ImGui::Text("Loaded asset path: %s, type: %s", mtd.FilePath.string().c_str(), AssetTypeToString(mtd.Type).c_str());
				}
			}
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}
private:
	memory::Ref<grapichs::FreeCamera> _defaultCamera = nullptr;

	memory::Ref<Scene> _defaultScene = nullptr;
	memory::Ref<BasicObject> _basicObject = nullptr;
	memory::Ref<PhysicsObject> _physicsObject = nullptr;
	Entity* _selectedEntity = nullptr;

	bool _cursorEnabled = false;
	bool _debugOverlayEnabled = false;
};

ae::Application* CreateApplication() {
	return new SandboxGame();
}