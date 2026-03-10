#pragma once
#include "ArchEngine/Utilities/UUID.h"
#include "ArchEngine/Core/Memory.h"
#include "Asset.h"
#include "AssetMetadata.h"
#include <map>
#include <filesystem>

namespace ae {
	using AssetMap = std::map<AssetHandle, memory::Ref<Asset>>;
	using AssetRegistry = std::map<AssetHandle, AssetMetadata>;
	using PackedAssetMap = std::map<AssetHandle, PackedAsset>;
	class AssetManagerBase {
	public:
		virtual ~AssetManagerBase() = default;
		virtual memory::Ref<Asset> GetAsset(AssetHandle handle) = 0;
		virtual memory::Ref<Asset> GetMemoryAsset(AssetHandle handle) = 0;
		virtual bool IsAssetHandleValid(AssetHandle handle) const = 0;
		virtual bool IsAssetLoaded(AssetHandle handle) const = 0;
		virtual AssetType GetAssetType(AssetHandle handle) const = 0;
		virtual AssetMap GetLoadedAssets() const = 0;
		virtual AssetMap GetLoadedAssetsWithType(AssetType type) const { return AssetMap(); };
		virtual AssetHandle AddOnlyMemoryAsset(const memory::Ref<Asset>& asset) { return INVALID_ASSET_HANDLE; }
		virtual AssetHandle ImportAsset(const std::filesystem::path& path) { return INVALID_ASSET_HANDLE; };
		virtual AssetMetadata GetMetadata(AssetHandle handle) const { return AssetMetadata(); };
	};
}