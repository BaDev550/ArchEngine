#include "ArchPch.h"
#include "AssetManagerRuntime.h"
#include "AssetImporter.h"

namespace ae {
	AssetManagerRuntime::AssetManagerRuntime(const std::filesystem::path& pakFile) {
		_packFile.open(pakFile, std::ios::binary);
		if (!_packFile.is_open()) {
			Logger_app::error("Failed to open PAK file: {}", pakFile.string());
			return;
		}

		uint32_t magic = 0;
		uint32_t assetCount = 0;
		_packFile.read((char*)&magic, sizeof(uint32_t));
		if (magic != ARCH_ENGINE_MAGIC) {
			Logger_app::error("Failed to read magic from file: {}", pakFile.string());
			return;
		}
		_packFile.read((char*)&assetCount, sizeof(uint32_t));
		for (uint32_t i = 0; i < assetCount; i++) {
			AssetHandle handle;
			PackedAsset packedAsset;

			_packFile.read((char*)&handle, sizeof(uint64_t));
			_packFile.read((char*)&packedAsset.Offset, sizeof(uint64_t));
			_packFile.read((char*)&packedAsset.Size, sizeof(uint64_t));
			_packFile.read((char*)&packedAsset.Type, sizeof(AssetType));
			_packedAssets[handle] = packedAsset;
		}
		AssetImporter::Init();
		Logger_app::info("Runtime asset manager initialized with pack file: {}", pakFile.string());
	}

	AssetManagerRuntime::~AssetManagerRuntime() {
		AssetImporter::Destroy();
		if (_packFile.is_open()) _packFile.close();
	}

	memory::Ref<Asset> AssetManagerRuntime::GetAsset(AssetHandle handle) {
		if (IsAssetLoaded(handle))
			return _loadedAssets[handle];

		if (IsAssetHandleValid(handle)) {
			const auto& packedAsset = _packedAssets[handle];
			AssetMetadata metadata;
			metadata.Handle = handle;
			metadata.Type = packedAsset.Type;
			std::vector<uint8_t> buffer(packedAsset.Size);
			_packFile.seekg(packedAsset.Offset);
			_packFile.read((char*)buffer.data(), packedAsset.Size);

			//memory::Ref<Asset> asset;
			return nullptr;
		}
	}

	memory::Ref<Asset> AssetManagerRuntime::GetMemoryAsset(AssetHandle handle) {
		return _memoryAssets[handle];
	}

	bool AssetManagerRuntime::IsAssetHandleValid(AssetHandle handle) const {
		if (_packedAssets.find(handle) != _packedAssets.end())
			return true;
		return false;
	}

	bool AssetManagerRuntime::IsAssetLoaded(AssetHandle handle) const {
		if (_loadedAssets.find(handle) != _loadedAssets.end())
			return true;
		return false;
	}

	AssetType AssetManagerRuntime::GetAssetType(AssetHandle handle) const {
		if (IsAssetHandleValid(handle))
			return _packedAssets.at(handle).Type;
	}
}