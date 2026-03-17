#pragma once
#include "Asset.h"
#include "AssetMetadata.h"
#include "AssetManagerEditor.h"

namespace ae {
	class AssetManager {
	public:
		static void Init();
		static void Destroy();

		template<typename T, typename... Args>
		static memory::Ref<T> GetAsset(AssetHandle handle) {
			return s_EditorAssetManager->GetAsset(handle).As<T>();
		}

		template<typename T, typename... Args>
		static memory::Ref<T> Create(const std::string& filename, Args&&... args) {
			return s_EditorAssetManager->Create<T>(filename, std::forward<Args>(args)...);
		}

		static AssetMap GetLoadedAssetsWithType(AssetType type) { return s_EditorAssetManager->GetAssetsByType(type); }
		static AssetMap GetLoadedAssets() { return s_EditorAssetManager->GetLoadedAssets(); }
		static memory::Ref<Asset> GetMemoryAsset(AssetHandle handle) { return s_EditorAssetManager->GetMemoryAsset(handle); }
		static bool IsAssetHandleValid(AssetHandle handle) { return s_EditorAssetManager->IsAssetHandleValid(handle); }
		static bool IsAssetLoaded(AssetHandle handle) { return s_EditorAssetManager->IsAssetLoaded(handle); }
		static AssetType GetAssetType(AssetHandle handle) { return s_EditorAssetManager->GetAssetType(handle); }
		static AssetHandle ImportAsset(const std::string& filename, AssetType type = AssetType::Unknown) { return s_EditorAssetManager->ImportAsset(filename, type); }
		static AssetMetadata GetAssetMetadata(AssetHandle handle) { return s_EditorAssetManager->GetMetadata(handle); }

		template<typename T>
		static AssetHandle AddMemoryOnlyAsset(memory::Ref<T> asset) {
			static_assert(std::is_base_of<Asset, T>::value, "T Must inherit from asset");
			asset->SetAssetHandle(UUID());
			s_EditorAssetManager->AddMemoryAsset(asset);
			return asset->GetAssetHandle();
		}
	private:
		static memory::Scope<AssetManagerEditor> s_EditorAssetManager;
	};
}