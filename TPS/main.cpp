#include <ArchEngine.h>

#include <ArchEngine/Grapichs/FreeCamera.h>
#include <ArchEngine/GUI/DebugOverlay.h>
#include <ArchEngine/Scene/SceneSerializer.h>
#include "Common/Defines.h"
#include <iostream>

#include "Entities/Entity_Player.h"
#include "Entities/Entity_Cube.h"

using namespace ae;
using namespace ae::key;
using namespace ae::memory;
using namespace ae::grapichs;

class TPS : public Application {
public:
	TPS(const ApplicationSpecifications& specs) : Application(specs) {}
	virtual void ApplicationStarted() override {
		_mainScene = Ref<Scene>::Create("MainScene");
		_mainScene->CreateEntity<Entity_Skybox>(GetResourcePath("Textures/Skybox_01.hdr"));
		_mainScene->CreateEntity<Entity_DirectionalLight>();

		_mainCamera = _mainScene->CreateEntity<Entity_Camera>();
		_player = _mainScene->CreateEntity<Entity_Player>();
		_mainScene->CreateEntity<Entity_Cube>();

		_defaultCamera = Ref<FreeCamera>::Create();
	};
	
	virtual void ApplicationUpdate() override {
		if (Input::IsKeyJustPressed(key::Tab)) {
			_cursorEnabled = !_cursorEnabled;
			_defaultCamera->SetFirstMouse();
			_defaultCamera->SetProccessingMouse(!_cursorEnabled);
			_window->SetCursor(_cursorEnabled);
		}

		if (Input::IsKeyJustPressed(key::F1)) {
			_debugOverlay.ToggleOverlay();
			_mainScene->GetRenderer().GetSceneData().DrawDebugShapes = _debugOverlay.OverlayEnabled();
		}

		if (Input::IsKeyJustPressed(key::F2)) {
			SaveActiveScene();
		}
		if (Input::IsKeyJustPressed(key::F3)) {
			LoadScene();
		}

		Renderer::BeginFrame();
		ImGuiRenderer::Begin();
		_defaultCamera->Update(_deltaTime);

		_mainScene->OnEditorUpdate(_defaultCamera, _deltaTime);

		Renderer::BeginDefaultRenderPass();
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
		ImGui::Image((ImTextureID)(VkDescriptorSet)Renderer::GetFinalImageOfScene(_mainScene), viewportPanelSize);
		ImGui::End();
		_debugOverlay.Draw(_mainScene);
		Renderer::EndDefaultRenderPass();
		ImGuiRenderer::End();
		Renderer::EndFrame();
	};
private:
	Ref<FreeCamera> _defaultCamera = nullptr;
	GUI::DebugOverlay _debugOverlay;
	bool _cursorEnabled = false;

	Ref<Scene> _mainScene = nullptr;
	Ref<Entity_Camera> _mainCamera = nullptr;

	Ref<Entity_Player> _player = nullptr;

	void SaveActiveScene() {
		SceneSerializer serializer(_mainScene);
		serializer.Serialize(_mainScene->GetName() + ".scene");
	}

	void LoadScene() {
		SceneSerializer serializer(_mainScene);
		_debugOverlay.SetSelectedEntity(nullptr);
		serializer.Deserialize(_mainScene->GetName() + ".scene");
	}
};

Application* CreateApplication() {
	ApplicationSpecifications specs{};
	specs.Title = "FPS";
	specs.Width = 1250;
	specs.Height = 720;
	return new TPS(specs);
}