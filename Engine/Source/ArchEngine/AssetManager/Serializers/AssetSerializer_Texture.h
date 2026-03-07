#pragma once

#include "AssetSerializer.h"

namespace ae {
	class AssetSerializer_Texture : public AssetSerializer {
	public:
		virtual void Serialize(const AssetMetadata& metadata, const memory::Ref<Asset>& asset) override {}
		virtual bool TryLoadData(const AssetMetadata& metadata, memory::Ref<Asset>& asset) override;
	};
}