#include "ArchPch.h"
#include "AssetManager.h"

namespace ae {
	memory::Scope<AssetManagerEditor> AssetManager::s_AssetManagerEditor = nullptr;
	memory::Scope<AssetManagerRuntime> AssetManager::s_AssetManagerRuntime = nullptr;
	void AssetManager::Init(const std::filesystem::path& pakFilePath) {
		if (pakFilePath.empty()) {
			s_AssetManagerType = AssetManagerType::Editor;
			s_AssetManagerEditor = memory::MakeScope<AssetManagerEditor>();
		}
		else {
			s_AssetManagerType = AssetManagerType::Runtime;
			s_AssetManagerRuntime = memory::MakeScope<AssetManagerRuntime>(pakFilePath);
		}
	}

	void AssetManager::Destroy() {
		s_AssetManagerEditor = nullptr;
		s_AssetManagerRuntime = nullptr;
	}

	AssetMap AssetManager::GetLoadedAssetsWithType(AssetType type) { 
		if (s_AssetManagerType == AssetManagerType::Editor)
			return s_AssetManagerEditor->GetLoadedAssetsWithType(type);
		return s_AssetManagerRuntime->GetLoadedAssetsWithType(type);
	}

	AssetMap AssetManager::GetLoadedAssets() { 
		if (s_AssetManagerType == AssetManagerType::Editor)
			return s_AssetManagerEditor->GetLoadedAssets();
		return s_AssetManagerRuntime->GetLoadedAssets();
	}

	memory::Ref<Asset> AssetManager::GetMemoryAsset(AssetHandle handle) { 
		if (s_AssetManagerType == AssetManagerType::Editor)
			return s_AssetManagerEditor->GetMemoryAsset(handle);
		return s_AssetManagerRuntime->GetMemoryAsset(handle);
	}

	bool AssetManager::IsAssetHandleValid(AssetHandle handle) { 
		if (s_AssetManagerType == AssetManagerType::Editor)
			return s_AssetManagerEditor->IsAssetHandleValid(handle); 
		return s_AssetManagerRuntime->IsAssetHandleValid(handle);
	}

	bool AssetManager::IsAssetLoaded(AssetHandle handle) { 
		if (s_AssetManagerType == AssetManagerType::Editor)
			return s_AssetManagerEditor->IsAssetLoaded(handle);
		return s_AssetManagerRuntime->IsAssetLoaded(handle);
	}

	AssetType AssetManager::GetAssetType(AssetHandle handle) { 
		if (s_AssetManagerType == AssetManagerType::Editor)
			return s_AssetManagerEditor->GetAssetType(handle);
		return s_AssetManagerRuntime->GetAssetType(handle);
	}

	AssetHandle AssetManager::ImportAsset(const std::string& filename) { 
		if (s_AssetManagerType == AssetManagerType::Editor)
			return s_AssetManagerEditor->ImportAsset(filename);
		return s_AssetManagerRuntime->ImportAsset(filename);
	}

	AssetMetadata AssetManager::GetAssetMetadata(AssetHandle handle) { 
		if (s_AssetManagerType == AssetManagerType::Editor)
			return s_AssetManagerEditor->GetMetadata(handle);
		return s_AssetManagerRuntime->GetMetadata(handle);
	}
}