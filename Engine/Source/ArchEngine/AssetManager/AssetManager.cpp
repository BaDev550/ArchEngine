#include "ArchPch.h"
#include "AssetManager.h"

namespace ae {
	memory::Scope<AssetManagerEditor> AssetManager::s_EditorAssetManager = nullptr;

	void AssetManager::Init() {
		s_EditorAssetManager = memory::MakeScope<AssetManagerEditor>();
	}

	void AssetManager::Destroy() {
		s_EditorAssetManager = nullptr;
	}
}