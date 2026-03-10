#pragma once
#include "Asset.h"
#include "AssetMetadata.h"
#include "AssetManagerEditor.h"
#include "AssetManagerRuntime.h"

namespace ae {
	enum class AssetManagerType {
		Editor,
		Runtime
	};

	class AssetManager {
	public:
		static void Init(const std::filesystem::path& pakFilePath = "");
		static void Destroy();

		template<typename T, typename... Args>
		static memory::Ref<T> GetAsset(AssetHandle handle) {
			return s_AssetManager->GetAsset(handle).As<T>();
		}

		template<typename T, typename... Args>
		static memory::Ref<T> Create(const std::string& filename, Args&&... args) {
			if (s_AssetManagerType != AssetManagerType::Editor) {
				Logger_app::error("AssetManager::Create can only be called in Editor mode!");
				return nullptr;
			}
			return s_AssetManagerEditorInstance->Create<T>(filename, std::forward<Args>(args)...);
		}

		static AssetMap GetLoadedAssetsWithType(AssetType type);
		static AssetMap GetLoadedAssets();
		static memory::Ref<Asset> GetMemoryAsset(AssetHandle handle);
		static bool IsAssetHandleValid(AssetHandle handle);
		static bool IsAssetLoaded(AssetHandle handle);
		static AssetType GetAssetType(AssetHandle handle);
		static AssetHandle ImportAsset(const std::string& filename);
		static AssetMetadata GetAssetMetadata(AssetHandle handle);
		static AssetHandle GetOrCreateAssetHandle(const std::string& filePath);

		static AssetManagerEditor* GetEditorAssetManagerInstance() { return s_AssetManagerEditorInstance; }
		static AssetManagerBase* GetAssetManagerInstance() { return s_AssetManager.get(); }
		static AssetManagerType GetAssetManagerType() { return s_AssetManagerType; }

		template<typename T>
		static AssetHandle AddMemoryOnlyAsset(memory::Ref<T> asset) {
			static_assert(std::is_base_of<Asset, T>::value, "T Must inherit from asset");
			asset->SetAssetHandle(UUID());
			s_AssetManager->AddOnlyMemoryAsset(asset);
			return asset->GetAssetHandle();
		}
	private:
		static inline AssetManagerType s_AssetManagerType = AssetManagerType::Editor;
		static memory::Scope<AssetManagerBase> s_AssetManager;
		static AssetManagerEditor* s_AssetManagerEditorInstance;
	};
}