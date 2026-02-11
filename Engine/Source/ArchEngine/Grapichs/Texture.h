#pragma once
#include "RenderContext.h"

#include "ArchEngine/Core/Memory.h"
#include "ArchEngine/Utilities/DataBuffer.h"

namespace ae::grapichs {
	const bool IsDepthFormat(vk::Format format) {
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

	class Texture : public memory::RefCounted {
	public:
		virtual ~Texture() = default;
		vk::Format GetFormat() const { return _specs.Format; }
		vk::Filter GetFilter() const { return _specs.Filter; }
		vk::SamplerAddressMode GetWrapMode() const { return _specs.Wrap; }
		uint32_t GetWidth() const { return _specs.Width; }
		uint32_t GetHeight() const { return _specs.Width; }
		bool IsAttachment() const { return _specs.Attachment; }
	protected:
		TextureSpecification _specs;
	};
	
	class Texture2D : public Texture {
	public:
		Texture2D(const TextureSpecification& specs);
		Texture2D(const TextureSpecification& specs, const std::filesystem::path& path);
		Texture2D(const TextureSpecification& specs, DataBuffer data);
		~Texture2D();

		vk::Image GetImage() const { return _image; }
		vk::ImageView GetImageView() const { return _imageView; }
		vk::DescriptorImageInfo GetImageDescriptorInfo() const {
			return {
				.sampler = _imageSampler,
				.imageView = _imageView,
				.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal
			};
		}
	private:
		void LoadTexture(void* data, uint32_t width, uint32_t height, uint32_t channels);
		void CreateTexture();
		void CreateTextureImageView();
		void CreateTextureSampler();

		vk::Image _image;
		vk::DeviceMemory _imageMemory;
		vk::ImageView _imageView;
		vk::Sampler _imageSampler;

		RenderContext& _context;
	};
}