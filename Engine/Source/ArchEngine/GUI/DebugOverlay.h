#pragma once
#include "ArchEngine/AssetManager/AssetManager.h"
#include "ArchEngine/Objects/Entity.h"
#include "ArchEngine/Core/Application.h"
#include "ArchEngine/Grapichs/Renderer.h"
#include "ArchEngine/Scene/Scene.h"

#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ae::GUI {
	class DebugOverlay {
	public:
		DebugOverlay() = default;

		Entity* GetSelectedEntity() { return _selectedEntity; }
		bool IsEnabled() const { return _enabled; }
		bool DebugDraw() const { return _debugDraw; }
		void Toggle() { _enabled = !_enabled; }
		void SetSelectedEntity(Entity* selectedEntity) { _selectedEntity = selectedEntity; }

		void Draw(memory::Ref<Scene>& scene) {
			if (!_enabled) return;

			ImGuiViewport* mainViewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos({ mainViewport->WorkPos.x, mainViewport->WorkPos.y }, ImGuiCond_Always);
			ImGui::SetNextWindowSize({ mainViewport->WorkSize.x, mainViewport->WorkSize.y / 2.5f }, ImGuiCond_Always);
			ImGui::SetNextWindowBgAlpha(0.5f);

			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.6f);
			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoResize;

			if (ImGui::Begin("Level Editor", nullptr, windowFlags)) {
				ImGui::TextDisabled("ArchEngine Level Editor");
				ImGui::Separator();

				if (ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_DefaultOpen)) {
					if (ImGui::Button("Add Object")) { _objectCreator = !_objectCreator; }

					ImGui::Spacing();

					for (auto& [id, entity] : scene->GetEntities()) {
						std::string entityName = entity->GetName() + "##" + entity->GetID().ToString();
						if (ImGui::Selectable(entityName.c_str(), entity.Get() == _selectedEntity)) {
							_selectedEntity = entity.Get();
						}
					}
				}

				if (_objectCreator) {
					ImGui::Begin("Object Creator", &_objectCreator);
					for (auto& [type, func] : EntityFactory::GetRegistry()) {
						std::string lable = "Create " + type;
						if (ImGui::Button(lable.c_str(), ImVec2(ImGui::GetWindowSize().x, 20.0f)))
							scene->CreateEntity(type);
					}
					ImGui::End();
				}

				if (_selectedEntity) {
					if (ImGui::CollapsingHeader("Inspector", ImGuiTreeNodeFlags_DefaultOpen)) {
						ImGui::Text("ID: %d", _selectedEntity->GetID());
						ImGui::Separator();

						auto& transform = _selectedEntity->GetTransform();
						glm::vec3 euler = transform.GetEulerRotation();

						ImGui::Text("Transform");
						ImGui::DragFloat3("Position", glm::value_ptr(transform.Position), 0.1f);
						if (ImGui::DragFloat3("Rotation", glm::value_ptr(euler), 0.5f)) {
							transform.SetEulerRotation(euler);
						}
						ImGui::DragFloat3("Scale", glm::value_ptr(transform.Scale), 0.1f);
						ImGui::Separator();
					}
				}

				if (ImGui::CollapsingHeader("Engine")) {
					ImGui::Text("FPS: %.1f", 1.0f / Application::Get()->GetDeltaTime());
					ImGui::Text("Draw Calls: %d", Renderer::GetDrawCallCount());
					ImGui::Checkbox("Debug Draw Shapes", &_debugDraw);
					ImGui::Spacing();

					if (ImGui::TreeNode("Loaded Assets")) {
						auto loadedAssets = AssetManager::GetLoadedAssets();
						for (const auto& [assethandle, asset] : loadedAssets) {
							AssetMetadata mtd = AssetManager::GetAssetMetadata(assethandle);
							ImGui::BulletText("%s (%s)", mtd.FilePath.filename().string().c_str(), AssetTypeToString(mtd.Type).c_str());
						}
						ImGui::TreePop();
					}
				}
			}
			ImGui::End();
			ImGui::PopStyleVar();
		}

	private:
		Entity* _selectedEntity = nullptr;
		bool _enabled = false;
		bool _debugDraw = false;
		bool _objectCreator = false;
	};
}