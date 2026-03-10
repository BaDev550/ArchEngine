#pragma once

#include "AssetManagerBase.h"
#include <fstream>

namespace ae {
	class AssetManagerRuntime : public AssetManagerBase {
	public:
		AssetManagerRuntime(const std::filesystem::path& pakFile);
		virtual ~AssetManagerRuntime();

		virtual memory::Ref<Asset> GetAsset(AssetHandle handle) override;
		virtual memory::Ref<Asset> GetMemoryAsset(AssetHandle handle) override;
		virtual bool IsAssetHandleValid(AssetHandle handle) const override;
		virtual bool IsAssetLoaded(AssetHandle handle) const override;
		virtual AssetType GetAssetType(AssetHandle handle) const override;
		virtual AssetMap GetLoadedAssets() const override { return _loadedAssets; }
	private:
		PackedAssetMap _packedAssets;
		AssetMap _loadedAssets;
		AssetMap _memoryAssets;
		std::ifstream _packFile;
	};
}