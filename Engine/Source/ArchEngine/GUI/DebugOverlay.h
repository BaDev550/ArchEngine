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
		bool OverlayEnabled() const { return _overlayEnabled; }
		void ToggleOverlay() { _overlayEnabled = !_overlayEnabled; }
		void SetSelectedEntity(Entity* selectedEntity) { _selectedEntity = selectedEntity; }
		void Draw(memory::Ref<Scene>& scene) {
			if (_overlayEnabled) {
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
					ImGui::Text("ArchEngine Debug overlay panel");
					ImGui::Separator();

					if (ImGui::CollapsingHeader("Render stats")) {
						ImGui::Text("FPS: %.1f", 1.0f / Application::Get()->GetDeltaTime());
						ImGui::Text("Draw Calls: %d", Renderer::GetDrawCallCount());
						ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
					}
					if (ImGui::CollapsingHeader("Scene Hierarchy")) {
						if (ImGui::CollapsingHeader("Create Entity")) {
							for (auto& [type, func] : EntityFactory::GetRegistry()) {
								std::string lable = "Spawn " + type;
								if (ImGui::Button(lable.c_str()))
									scene->CreateEntity(type);
							}
						}
						for (auto& [id, entity] : scene->GetEntities()) {
							std::string entityName = entity->GetName() + "##" + entity->GetID().ToString();
							if (ImGui::Selectable(entityName.c_str(), entity == _selectedEntity)) {
								_selectedEntity = entity.Get();
							}
						}
						if (_selectedEntity) {
							ImGui::Separator();
							auto& transform = _selectedEntity->GetTransform();
							glm::vec3 euler = transform.GetEulerRotation();
							ImGui::Text("Entity ID: %d", _selectedEntity->GetID());
							ImGui::DragFloat3("Position", glm::value_ptr(transform.Position), 0.1f);
							if (ImGui::DragFloat3("Rotation", glm::value_ptr(euler), 0.5f)) { transform.SetEulerRotation(euler);}
							ImGui::DragFloat3("Scale", glm::value_ptr(transform.Scale), 0.1f);
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
		}
	private:
		Entity* _selectedEntity = nullptr;
		bool _overlayEnabled = false;
	};
}