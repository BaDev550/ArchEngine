#include "ArchPch.h"
#include "AssetSerializer_Texture.h"

#include "ArchEngine/Grapichs/Texture.h"

namespace ae {
	bool AssetSerializer_Texture::TryLoadData(const AssetMetadata& metadata, memory::Ref<Asset>& asset)
	{
		asset = memory::Ref<grapichs::Texture2D>::Create(grapichs::TextureSpecification(), metadata.FilePath);
		asset->SetAssetHandle(metadata.Handle);
		return true;
	}
}