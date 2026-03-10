#include "ArchPch.h"
#include "AssetManager.h"

namespace ae {
	memory::Scope<AssetManagerBase> AssetManager::s_AssetManager = nullptr;
	AssetManagerEditor* AssetManager::s_AssetManagerEditorInstance = nullptr;

	void AssetManager::Init(const std::filesystem::path& pakFilePath) {
		if (pakFilePath.empty()) {
			memory::Scope<AssetManagerEditor> editorManager = memory::MakeScope<AssetManagerEditor>();
			s_AssetManagerType = AssetManagerType::Editor;
			s_AssetManager = std::move(editorManager);
			s_AssetManagerEditorInstance = (AssetManagerEditor*)s_AssetManager.get();
		}
		else {
			s_AssetManager = memory::MakeScope<AssetManagerRuntime>(pakFilePath);
			s_AssetManagerType = AssetManagerType::Runtime;
			s_AssetManagerEditorInstance = nullptr;
		}
	}

	void AssetManager::Destroy() {
		s_AssetManager = nullptr;
		s_AssetManagerEditorInstance = nullptr;
	}

	AssetMap AssetManager::GetLoadedAssetsWithType(AssetType type) { 
		return s_AssetManager->GetLoadedAssetsWithType(type);
	}

	AssetMap AssetManager::GetLoadedAssets() { 
		return s_AssetManager->GetLoadedAssets();
	}

	memory::Ref<Asset> AssetManager::GetMemoryAsset(AssetHandle handle) { 
		return s_AssetManager->GetMemoryAsset(handle);
	}

	bool AssetManager::IsAssetHandleValid(AssetHandle handle) { 
		return s_AssetManager->IsAssetHandleValid(handle);
	}

	bool AssetManager::IsAssetLoaded(AssetHandle handle) { 
		return s_AssetManager->IsAssetLoaded(handle);
	}

	AssetType AssetManager::GetAssetType(AssetHandle handle) { 
		return s_AssetManager->GetAssetType(handle);
	}

	AssetHandle AssetManager::ImportAsset(const std::string& filename) { 
		return s_AssetManager->ImportAsset(filename);
	}

	AssetMetadata AssetManager::GetAssetMetadata(AssetHandle handle) { 
		return s_AssetManager->GetMetadata(handle);
	}

	AssetHandle AssetManager::GetOrCreateAssetHandle(const std::string& filePath) {
		std::string standardizedPath = std::filesystem::path(filePath).generic_string();
		uint64_t pathHash = HashString(standardizedPath);

		if (s_AssetManagerType == AssetManagerType::Editor) {
			if (!s_AssetManagerEditorInstance->IsAssetHandleValid(pathHash)) {
				Logger_app::info("Auto-importing code-requested asset: {}", standardizedPath);
				s_AssetManagerEditorInstance->ImportAssetWithHandle(standardizedPath, pathHash);
			}
			return pathHash;
		}
		else {
			return pathHash;
		}
	}
}