#pragma once
#include <filesystem>
#include "Asset.h"

namespace ae {
	enum class AssetLoadingState : uint8_t {
		NotLoaded = 0,
		Loading,
		Loaded,
		Failed
	};

	struct AssetMetadata {
		AssetHandle Handle = INVALID_ASSET_HANDLE;
		AssetType Type = INVALID_ASSET_TYPE;
		AssetLoadingState LoadingState = AssetLoadingState::NotLoaded;
		std::filesystem::path FilePath;

		bool IsValid() const { return Type != INVALID_ASSET_TYPE; }
	};
}