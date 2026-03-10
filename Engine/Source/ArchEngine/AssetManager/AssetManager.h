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
			if (s_AssetManagerType == AssetManagerType::Editor) 
				return s_AssetManagerEditor->GetAsset(handle).As<T>();
			return s_AssetManagerRuntime->GetAsset(handle).As<T>();
		}

		template<typename T, typename... Args>
		static memory::Ref<T> Create(const std::string& filename, Args&&... args) {
			return s_AssetManagerEditor->Create<T>(filename, std::forward<Args>(args)...);
		}

		static AssetMap GetLoadedAssetsWithType(AssetType type);
		static AssetMap GetLoadedAssets();
		static memory::Ref<Asset> GetMemoryAsset(AssetHandle handle);
		static bool IsAssetHandleValid(AssetHandle handle);
		static bool IsAssetLoaded(AssetHandle handle);
		static AssetType GetAssetType(AssetHandle handle);
		static AssetHandle ImportAsset(const std::string& filename);
		static AssetMetadata GetAssetMetadata(AssetHandle handle);

		static AssetManagerEditor* GetEditorAssetManager() { return s_AssetManagerEditor.get(); }
		static AssetManagerRuntime* GetRuntimeAssetManager() { return s_AssetManagerRuntime.get(); }

		template<typename T>
		static AssetHandle AddMemoryOnlyAsset(memory::Ref<T> asset) {
			static_assert(std::is_base_of<Asset, T>::value, "T Must inherit from asset");
			asset->SetAssetHandle(UUID());
			if (s_AssetManagerType == AssetManagerType::Editor) { s_AssetManagerEditor->AddOnlyMemoryAsset(asset); }
			else { s_AssetManagerRuntime->AddOnlyMemoryAsset(asset); }
			return asset->GetAssetHandle();
		}
	private:
		static inline AssetManagerType s_AssetManagerType = AssetManagerType::Editor;
		static memory::Scope<AssetManagerEditor> s_AssetManagerEditor;
		static memory::Scope<AssetManagerRuntime> s_AssetManagerRuntime;
	};
}