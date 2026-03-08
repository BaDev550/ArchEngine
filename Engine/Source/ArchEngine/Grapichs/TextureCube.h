#pragma once
#include "Texture.h"
#include "ArchEngine/Utilities/Bitmap.h"

namespace ae::grapichs {
	class TextureCube : public Texture {
	public:
        TextureCube(const TextureSpecification& specs, const std::vector<Bitmap>& faces);
        ~TextureCube();

        vk::Image GetImage() const { return _image; }
        vk::ImageView GetImageView() const { return _imageView; }
        vk::Sampler GetSampler() const { return _imageSampler; }
        virtual vk::DescriptorImageInfo& GetImageDescriptorInfo() override { return _imageInfo; }

        static AssetType GetStaticAssetType() { return AssetType::TextureCube; }
        virtual AssetType GetAssetType() const override { return GetStaticAssetType(); }
    private:
        void CreateTexture();
        void CreateTextureImageView();
        void CreateTextureSampler();

        vk::Image _image;
        vk::DeviceMemory _imageMemory;
        vk::ImageView _imageView;
        vk::Sampler _imageSampler;
        vk::DescriptorImageInfo _imageInfo;

        RenderContext& _context;
	};
}