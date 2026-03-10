#include "ArchPch.h"
#include "AssetManagerEditor.h"
#include "AssetExtensions.h"

#include <yaml-cpp/yaml.h>

namespace ae {
	AssetManagerEditor::AssetManagerEditor() {
		AssetImporter::Init();
		LoadAssetRegistry();
	}

	AssetManagerEditor::~AssetManagerEditor() {
		AssetImporter::Destroy();
		_assetRegistry.clear();
		_loadedAssets.clear();
	}

	memory::Ref<Asset> AssetManagerEditor::GetAsset(AssetHandle handle) {
		if (IsMemoryAsset(handle))
			return _memoryAssets[handle];

		memory::Ref<Asset> asset;
		AssetMetadata metadata = GetMetadata(handle);
		if (metadata.IsValid()) {
			if (metadata.LoadingState == AssetLoadingState::Loaded) { return _loadedAssets[handle]; }
			else {
				if (AssetImporter::TryLoadData(metadata, asset)) {
					metadata.LoadingState = AssetLoadingState::Loaded;
					_loadedAssets[metadata.Handle] = asset;
					SaveAssetRegistry();
				}
			}
		}
		return asset;
	}
	memory::Ref<Asset> AssetManagerEditor::GetMemoryAsset(AssetHandle handle) {
		if (auto it = _memoryAssets.find(handle); it != _memoryAssets.end())
			return it->second;
		return nullptr;
	}

	bool AssetManagerEditor::IsAssetHandleValid(AssetHandle handle) const {
		if (_assetRegistry.find(handle) != _assetRegistry.end())
			return true;
		return false;
	}
	bool AssetManagerEditor::IsAssetLoaded(AssetHandle handle) const {
		if (_loadedAssets.find(handle) != _loadedAssets.end())
			return true;
		return false;
	}
	AssetType AssetManagerEditor::GetAssetType(AssetHandle handle) const {
		if (IsAssetHandleValid(handle))
			return _assetRegistry.find(handle)->second.Type;
		return INVALID_ASSET_TYPE;
	}
	AssetMetadata AssetManagerEditor::GetMetadata(AssetHandle handle) const {
		if (_assetRegistry.find(handle) != _assetRegistry.end())
			return _assetRegistry.at(handle);
		return AssetMetadata();
	}
	AssetMetadata AssetManagerEditor::GetMetadata(const std::filesystem::path& path) const {
		for (auto& [handle, mtd] : _assetRegistry) {
			if (mtd.FilePath == path)
				return mtd;
		}
		return AssetMetadata();
	}
	AssetMap AssetManagerEditor::GetLoadedAssetsWithType(AssetType type) const {
		AssetMap resultMap;
		for (auto& [handle, asset] : _loadedAssets) {
			if (asset && asset->GetAssetType() == type)
				resultMap[handle] = asset;
		}
		return resultMap;
	}
	AssetHandle AssetManagerEditor::ImportAsset(const std::filesystem::path& path) {
		if (AssetMetadata loadedMetadata = GetMetadata(path); loadedMetadata.IsValid()) {
			memory::Ref<Asset> asset = GetAsset(loadedMetadata.Handle);
			_loadedAssets[loadedMetadata.Handle] = asset;
			return loadedMetadata.Handle;
		}

		AssetHandle handle = UUID();
		if (ImportAssetWithHandle(path, handle)) {
			return handle;
		}
		return INVALID_ASSET_HANDLE;
	}

	bool AssetManagerEditor::ImportAssetWithHandle(const std::filesystem::path& path, AssetHandle handle) {
		AssetMetadata mtd;
		memory::Ref<Asset> asset;
		mtd.Handle = handle;
		mtd.LoadingState = AssetLoadingState::Loading;
		mtd.FilePath = path;
		mtd.Type = s_ExtensionAssetMap[path.extension()];
		if (AssetImporter::TryLoadData(mtd, asset)) {
			mtd.LoadingState = AssetLoadingState::Loaded;
			_assetRegistry[mtd.Handle] = mtd;
			_loadedAssets[mtd.Handle] = asset;
			SaveAssetRegistry();
			Logger_app::info("Asset Loaded {}", mtd.FilePath.string());
			return true;
		}
		return false;
	}

	bool AssetManagerEditor::IsMemoryAsset(AssetHandle handle) const {
		if (_memoryAssets.find(handle) != _memoryAssets.end())
			return true;
		return false;
	}

	AssetHandle AssetManagerEditor::AddOnlyMemoryAsset(const memory::Ref<Asset>& asset) {
		_memoryAssets[asset->GetAssetHandle()] = asset;
		return asset->GetAssetHandle();
	}

	void AssetManagerEditor::SaveAssetRegistry(){
		YAML::Emitter out;
		out << YAML::BeginMap;

		out << YAML::Key << "Assets" << YAML::BeginSeq;
		for (auto& [handle, entry] : _assetRegistry) {
			out << YAML::BeginMap;
			out << YAML::Key << "Handle" << YAML::Value << handle;
			out << YAML::Key << "Path" << YAML::Value << entry.FilePath.string();
			out << YAML::Key << "Type" << YAML::Value << AssetTypeToString(entry.Type);
			out << YAML::EndMap;
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(_assetRegistryPath.string());
		fout << out.c_str();
	}
	void AssetManagerEditor::LoadAssetRegistry(){
		if (!std::filesystem::exists(_assetRegistryPath))
			return;

		std::ifstream stream(_assetRegistryPath.string());
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		auto handles = data["Assets"];
		if (!handles)
			return;

		for (auto entry : handles) {
			std::string filePath = entry["Path"].as<std::string>();
			std::string strType = entry["Type"].as<std::string>();

			AssetMetadata metadata;
			metadata.Handle = entry["Handle"].as<uint64_t>();
			metadata.FilePath = filePath;
			metadata.Type = StringToAssetType(strType);

			if (metadata.Type == INVALID_ASSET_TYPE) {
				Logger_app::warn("Tried to load AssetType::None asset");
				continue;
			}

			_assetRegistry[metadata.Handle] = metadata;
		}
		Logger_app::info("Asset registry loaded");
	}

	void AssetManagerEditor::CompileIntoPakFile(const std::filesystem::path& outPath) {
		std::ofstream pack(outPath, std::ios::binary);
		uint32_t magic = ARCH_ENGINE_MAGIC;
		uint32_t assetCount = static_cast<uint32_t>(_assetRegistry.size());
		pack.write((char*)&magic, sizeof(uint32_t));
		pack.write((char*)&assetCount, sizeof(uint32_t));

		uint64_t indexStart = pack.tellp();
		for (auto& [handle, mtd] : _assetRegistry) {
			uint64_t zero = 0;
			pack.write((char*)&handle, sizeof(uint64_t));
			pack.write((char*)&zero, sizeof(uint64_t));
			pack.write((char*)&zero, sizeof(uint64_t));
			pack.write((char*)&mtd.Type, sizeof(AssetType));
		}

		struct TempEntry { uint64_t offset; uint64_t size; };
		std::map<AssetHandle, TempEntry> finalEntries;

		for (auto& [handle, mtd] : _assetRegistry) {
			std::ifstream assetFile(mtd.FilePath, std::ios::binary | std::ios::ate);
			uint64_t size = assetFile.tellg();
			uint64_t offset = pack.tellp();

			assetFile.seekg(0);
			std::vector<char> buffer(size);
			assetFile.read(buffer.data(), size);
			pack.write(buffer.data(), size);
			finalEntries[handle] = { offset, size };
		}

		pack.seekp(indexStart);
		for (auto& [handle, mtd] : _assetRegistry) {
			pack.write((char*)&handle, sizeof(uint64_t));
			pack.write((char*)&finalEntries[handle].offset, sizeof(uint64_t));
			pack.write((char*)&finalEntries[handle].size, sizeof(uint64_t));
			pack.write((char*)&mtd.Type, sizeof(AssetType));
		}
		Logger_app::info("PAK file created at: {}", outPath.string());
	}
}