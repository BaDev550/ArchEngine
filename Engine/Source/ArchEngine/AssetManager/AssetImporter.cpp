#include "ArchPch.h"
#include "AssetImporter.h"

#include "Serializers/AssetSerializer_Mesh.h"
#include "Serializers/AssetSerializer_Texture.h"

namespace ae {
	std::unordered_map<AssetType, memory::Scope<AssetSerializer>> AssetImporter::s_serializers;
	void AssetImporter::Init() {
		s_serializers.clear();
		s_serializers[AssetType::MeshSource] = memory::MakeScope<AssetSerializer_MeshSource>();
		s_serializers[AssetType::StaticMesh] = memory::MakeScope<AssetSerializer_StaticMesh>();
		s_serializers[AssetType::EnviromentMap] = memory::MakeScope<AssetSerializer_Enviroment>();
		s_serializers[AssetType::Texture] = memory::MakeScope<AssetSerializer_Texture>();
	}
	void AssetImporter::Destroy() {
		s_serializers.clear();
	}
	void AssetImporter::Serialize(const AssetMetadata& metadata, const memory::Ref<Asset>& asset){
		if (s_serializers.find(metadata.Type) != s_serializers.end()) {
			s_serializers[metadata.Type]->Serialize(metadata, asset);
		}
		else {
			Logger_app::error("No serializer found for asset type {0}", AssetTypeToString(metadata.Type));
		}
	}
	bool AssetImporter::TryLoadData(const AssetMetadata& metadata, memory::Ref<Asset>& asset) {
		if (s_serializers.find(metadata.Type) != s_serializers.end()) {
			return s_serializers[metadata.Type]->TryLoadData(metadata, asset);
		}
		Logger_app::error("No serializer found for asset type {0}", AssetTypeToString(metadata.Type));
	}
	bool AssetImporter::TryLoadFromBuffer(const AssetMetadata& metadata, const std::vector<uint8_t>& buffer, memory::Ref<Asset>& asset) {
		if (s_serializers.find(metadata.Type) != s_serializers.end()) {
			return s_serializers[metadata.Type]->TryLoadFromBuffer(metadata.Handle, buffer, asset);
		}
		Logger_app::error("No serializer found for asset type {0}", AssetTypeToString(metadata.Type));
	}
}