#include "ArchPch.h"
#include "AssetSerializer_Texture.h"

#include "ArchEngine/Grapichs/Texture.h"
#include "ArchEngine/Grapichs/TextureCube.h"
#include "ArchEngine/Utilities/Bitmap.h"
#include "ArchEngine/Utilities/EctCubemap.h"

#include <stb_image.h>

namespace ae {
	bool AssetSerializer_Texture::TryLoadData(const AssetMetadata& metadata, memory::Ref<Asset>& asset)
	{
		asset = memory::Ref<grapichs::Texture2D>::Create(grapichs::TextureSpecification(), metadata.FilePath);
		asset->SetAssetHandle(metadata.Handle);
		return true;
	}

	bool AssetSerializer_TextureCube::TryLoadData(const AssetMetadata& metadata, memory::Ref<Asset>& asset)
	{
        int width, height, channels;
        void* data = nullptr;
        BitmapFormat format;
        grapichs::TextureSpecification specs;

        if (stbi_is_hdr(metadata.FilePath.string().c_str())) {
            data = stbi_loadf(metadata.FilePath.string().c_str(), &width, &height, &channels, 4);
            format = BitmapFormat::Float;
            specs.Format = vk::Format::eR32G32B32A32Sfloat;
        }
        else {
            data = stbi_load(metadata.FilePath.string().c_str(), &width, &height, &channels, 4);
            format = BitmapFormat::UnsignedByte;
            specs.Format = vk::Format::eR8G8B8A8Unorm;
        }

        if (data) {
            Bitmap equirectBitmap(width, height, 4, format, data);

            std::vector<Bitmap> cubemapFaces;
            ConvertEquirectangularImageToCubemap(equirectBitmap, cubemapFaces);
            asset = memory::Ref<grapichs::TextureCube>::Create(specs, cubemapFaces);
            stbi_image_free(data);
            return true;
        }
        else {
            Logger_renderer::error("Failed to load Equirectangular image for Cubemap: {}", metadata.FilePath.string());
            return false;
        }
	}
}