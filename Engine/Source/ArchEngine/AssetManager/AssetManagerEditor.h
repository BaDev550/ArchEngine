#pragma once
#include "Asset.h"
#include "AssetMetadata.h"
#include "AssetImporter.h"
#include "AssetManagerBase.h"
#include <filesystem>

namespace ae {
	class AssetManagerEditor : AssetManagerBase {
	public:
		AssetManagerEditor();
		~AssetManagerEditor();
		virtual memory::Ref<Asset> GetAsset(AssetHandle handle) override;
		virtual memory::Ref<Asset> GetMemoryAsset(AssetHandle handle) override;
		virtual bool IsAssetHandleValid(AssetHandle handle) const override;
		virtual bool IsAssetLoaded(AssetHandle handle) const override;
		virtual AssetType GetAssetType(AssetHandle handle) const override;
		virtual AssetMetadata GetMetadata(AssetHandle handle) const override;
		AssetMetadata GetMetadata(const std::filesystem::path& path) const;
		AssetMap GetMemoryAssets() const { return _memoryAssets; }

		virtual AssetMap GetLoadedAssets() const override { return _loadedAssets; };
		virtual AssetMap GetLoadedAssetsWithType(AssetType type) const override;
		virtual AssetHandle AddOnlyMemoryAsset(const memory::Ref<Asset>& asset) override;
		virtual AssetHandle ImportAsset(const std::filesystem::path& path) override;
		bool IsMemoryAsset(AssetHandle handle) const;

		void SaveAssetRegistry();
		void LoadAssetRegistry();
		void CompileIntoPakFile(const std::filesystem::path& outPath);

		template<typename T, typename... Args>
		memory::Ref<T> Create(const std::filesystem::path& path, Args&&... args) {
			static_assert(std::is_base_of<Asset, T>::value, "T must be a subclass of Asset");
			if (AssetMetadata loadedMetadata = GetMetadata(path); loadedMetadata.IsValid()) {
				memory::Ref<Asset> asset = GetAsset(loadedMetadata.Handle);
				_assetRegistry[loadedMetadata.Handle] = loadedMetadata;
				_loadedAssets[asset->GetAssetHandle()] = asset;
				return asset;
			}
			AssetMetadata metadata;

			metadata.Handle = UUID();
			metadata.FilePath = path;
			metadata.LoadingState = AssetLoadingState::Loaded;
			metadata.Type = T::GetStaticAssetType();
			_assetRegistry[metadata.Handle] = metadata;

			memory::Ref<T> asset = memory::Ref<T>::Create(std::forward<Args>(args)...);
			asset->SetAssetHandle(metadata.Handle);
			_loadedAssets[asset->GetAssetHandle()] = asset;
			AssetImporter::Serialize(metadata, asset);
			SaveAssetRegistry();
			return asset;
		}
	private:
		AssetRegistry _assetRegistry;
		AssetMap _loadedAssets;
		AssetMap _memoryAssets;
		const std::filesystem::path _assetRegistryPath = "AssetRegistry.rgs";
	};
}