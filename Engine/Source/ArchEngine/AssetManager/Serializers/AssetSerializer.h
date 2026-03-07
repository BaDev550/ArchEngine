#pragma once
#include "ArchEngine/AssetManager/AssetMetadata.h"
#include "ArchEngine/Core/Memory.h"
#include <iostream>

namespace ae {
	class AssetSerializer {
	public:
		virtual ~AssetSerializer() = default;
		virtual void Serialize(const AssetMetadata& metadata, const memory::Ref<Asset>& asset) = 0;
		virtual bool TryLoadData(const AssetMetadata& metadata, memory::Ref<Asset>& asset) = 0;
	};
}