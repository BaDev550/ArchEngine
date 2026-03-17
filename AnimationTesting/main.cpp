#include <iostream>
#include <memory>
#include <ArchEngine/Core/Application.h>
#include <ArchEngine/Grapichs/Renderer.h>
#include <ArchEngine/Grapichs/FreeCamera.h>
#include <ArchEngine/Core/EntryPoint.h>
#include <ArchEngine/Core/Input.h>
#include <ArchEngine/Scene/Scene.h>
#include <ArchEngine/Scene/SceneSerializer.h>
#include <ArchEngine/AssetManager/AssetManager.h>

#include "Entities/PhysicsObject.h"
#include "Entities/Entity_Box.h"
#include "Entities/Entity_BasicModel.h"
#include "Entities/Entity_Archer.h"
#include "Entities/Entity_TPCamera.h"
#include <ArchEngine/Objects/Entity_Camera.h>
#include <ArchEngine/Objects/Entity_Skybox.h>
#include <ArchEngine/Objects/Entity_DirectionalLight.h>

#include "ArchEngine/GUI/DebugOverlay.h"

#include <imgui.h>
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <backends/imgui_impl_vulkan.h>

using namespace ae;
class AnimationTestingGame : public Application {
public:
	AnimationTestingGame(const ApplicationSpecifications& specs) : Application(specs) {}
	~AnimationTestingGame() = default;

	virtual void ApplicationStarted() override {
		_defaultCamera = memory::Ref<FreeCamera>::Create();
		_defaultScene = memory::Ref<Scene>::Create();

		_skybox_01 = _defaultScene->CreateEntity<Entity_Skybox>("Resources/Textures/skybox.hdr");
		_defaultScene->CreateEntity<Entity_DirectionalLight>();
		_defaultScene->CreateEntity<Entity_Box>();
		_defaultScene->CreateEntity<Entity_BasicModel>("Resources/Models/barrel/barrel.gltf");
		_defaultScene->CreateEntity<Entity_BasicModel>("Resources/Models/barrel/barrel.gltf");
		_archer = _defaultScene->CreateEntity<Entity_Archer>();
		_tpCamera = _defaultScene->CreateEntity<Entity_TPCamera>();
		_tpCamera->SetTarget(_archer->GetID());
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
			_debugOverlay.ToggleOverlay();
			_defaultScene->GetRenderer().GetSceneData().DrawDebugShapes = _debugOverlay.OverlayEnabled();
		}

		if (Input::IsKeyJustPressed(key::F2)) {
			SaveActiveScene();
		}
		if (Input::IsKeyJustPressed(key::F3)) {
			LoadScene();
		}

		_defaultScene->OnRuntimeUpdate(_deltaTime);
		//_defaultScene->OnEditorUpdate(_defaultCamera, _deltaTime);

		Renderer::BeginDefaultRenderPass();
		DrawViewport();
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

		if (_debugOverlay.OverlayEnabled()) {
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
			
			auto rect = ImGui::GetWindowSize();
			auto pos = ImGui::GetWindowPos();
			ImGuizmo::SetRect(pos.x, pos.y, rect.x, rect.y);

			glm::mat4 view = _defaultCamera->GetView();
			glm::mat4 projection = _defaultCamera->GetProjection();
			projection[1][1] *= -1;

			if (auto entity = _debugOverlay.GetSelectedEntity()) {
				glm::mat4 modelTransform = entity->GetTransformMatrix();

				static ImGuizmo::OPERATION currentOp = ImGuizmo::TRANSLATE;
				if (Input::IsKeyPressed(key::W)) currentOp = ImGuizmo::TRANSLATE;
				if (Input::IsKeyPressed(key::E)) currentOp = ImGuizmo::ROTATE;
				if (Input::IsKeyPressed(key::R)) currentOp = ImGuizmo::SCALE;

				ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), currentOp, ImGuizmo::LOCAL, glm::value_ptr(modelTransform));

				if (ImGuizmo::IsUsing()) {
					entity->GetTransform().SetTransform(modelTransform);
				}
			}
		}
		_debugOverlay.Draw(_defaultScene);

		ImGui::End();
	}

	void SaveActiveScene() {
		ae::SceneSerializer serializer(_defaultScene);
		serializer.Serialize(_defaultScene->GetName() + ".scene");
	}

	void LoadScene() {
		ae::SceneSerializer serializer(_defaultScene);
		_debugOverlay.SetSelectedEntity(nullptr);
		serializer.Deserialize(_defaultScene->GetName() + ".scene");
	}
private:
	memory::Ref<grapichs::FreeCamera> _defaultCamera = nullptr;
	memory::Ref<Scene> _defaultScene = nullptr;

	memory::Ref<Entity_Archer> _archer = nullptr;
	memory::Ref<Entity_TPCamera> _tpCamera = nullptr;
	memory::Ref<Entity_Skybox> _skybox_01 = nullptr;

	bool _cursorEnabled = false;
	GUI::DebugOverlay _debugOverlay;
};

ae::Application* CreateApplication() {
	ApplicationSpecifications specs{};
	specs.Title = "Animation Playground";
	specs.Width = 1250;
	specs.Height = 720;
	return new AnimationTestingGame(specs);
}