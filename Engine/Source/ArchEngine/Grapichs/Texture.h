#pragma once
#include "RenderContext.h"
#include <filesystem>

#include "ArchEngine/Core/Memory.h"
#include "ArchEngine/Utilities/DataBuffer.h"
#include "ArchEngine/AssetManager/Asset.h"

namespace ae::grapichs {
	static bool IsDepthFormat(vk::Format format) {
		if (format == vk::Format::eD32Sfloat || format == vk::Format::eD24UnormS8Uint)
			return true;
		return false;
	}

	struct TextureSpecification {
		vk::Format Format = vk::Format::eR32G32B32A32Sfloat;
		vk::Filter Filter = vk::Filter::eLinear;
		vk::SamplerAddressMode Wrap = vk::SamplerAddressMode::eClampToEdge;
		uint32_t Width = 1;
		uint32_t Height = 1;
		uint32_t MipMapLevels = 1;
		bool Attachment = false;
		bool GenerateMipMap = false;
	};

	class Texture : public Asset {
	public:
		virtual ~Texture() = default;
		virtual vk::DescriptorImageInfo& GetImageDescriptorInfo() = 0;

		vk::Format GetFormat() const { return _specs.Format; }
		vk::Filter GetFilter() const { return _specs.Filter; }
		vk::SamplerAddressMode GetWrapMode() const { return _specs.Wrap; }
		uint32_t GetWidth() const { return _specs.Width; }
		uint32_t GetHeight() const { return _specs.Width; }
		bool IsAttachment() const { return _specs.Attachment; }
		static AssetType GetStaticAssetType() { return AssetType::Texture; }
		virtual AssetType GetAssetType() const override { return GetStaticAssetType(); }
	protected:
		TextureSpecification _specs;
	};
	
	class Texture2D : public Texture {
	public:
		Texture2D(const TextureSpecification& specs);
		Texture2D(const TextureSpecification& specs, const std::filesystem::path& path);
		Texture2D(const TextureSpecification& specs, vk::Image& image, vk::ImageView& imageView);
		Texture2D(const TextureSpecification& specs, DataBuffer data);
		~Texture2D();

		vk::Image GetImage() const { return _image; }
		vk::ImageView GetImageView() const { return _imageView; }
		vk::Sampler GetSampler() const { return _imageSampler; }
		vk::DescriptorSet GetImGuiTexture() { return _imguiImage; }
		virtual vk::DescriptorImageInfo& GetImageDescriptorInfo() override { return _imageInfo; }
	private:
		void LoadTexture(void* data, uint32_t width, uint32_t height, uint32_t channels);
		void CreateTexture();
		void CreateTextureImageView();
		void CreateTextureSampler();

		std::string _path;
		vk::Image _image;
		vk::DeviceMemory _imageMemory;
		vk::ImageView _imageView;
		vk::Sampler _imageSampler;
		vk::DescriptorSet _imguiImage;
		vk::DescriptorImageInfo _imageInfo;
		bool _ownsResources = true;

		RenderContext& _context;
	};
}