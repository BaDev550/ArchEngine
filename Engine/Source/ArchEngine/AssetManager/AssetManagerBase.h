#pragma once
#include "ArchEngine/Utilities/UUID.h"
#include "ArchEngine/Core/Memory.h"
#include "Asset.h"
#include "AssetMetadata.h"
#include <map>

namespace ae {
	using AssetMap = std::map<AssetHandle, memory::Ref<Asset>>;
	using AssetRegistry = std::map<AssetHandle, AssetMetadata>;
	class AssetManagerBase {
	public:
		virtual ~AssetManagerBase() = default;
		virtual memory::Ref<Asset> GetAsset(AssetHandle handle) = 0;
		virtual memory::Ref<Asset> GetMemoryAsset(AssetHandle handle) = 0;
		virtual bool IsAssetHandleValid(AssetHandle handle) const = 0;
		virtual bool IsAssetLoaded(AssetHandle handle) const = 0;
		virtual AssetType GetAssetType(AssetHandle handle) const = 0;
	};
}