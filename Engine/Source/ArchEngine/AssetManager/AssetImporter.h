#pragma once

#include "Serializers/AssetSerializer.h"
#include "AssetMetadata.h"
#include <unordered_map>

namespace ae {
	class AssetImporter {
	public:
		static void Init();
		static void Destroy();
		static void Serialize(const AssetMetadata& metadata, const memory::Ref<Asset>& asset);
		static bool TryLoadData(const AssetMetadata& metadata, memory::Ref<Asset>& asset);
		static bool TryLoadFromBuffer(const AssetMetadata& metadata, const std::vector<uint8_t>& buffer, memory::Ref<Asset>& asset);
	private:
		static std::unordered_map<AssetType, memory::Scope<AssetSerializer>> s_serializers;
	};
}