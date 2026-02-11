#include "ArchPch.h"
#include "Texture.h"
#include "Buffer.h"
#include "ShaderTypes.h"
#include "ArchEngine/Core/Application.h"

#include <stb_image.h>

namespace ae::grapichs {
	void Texture2D::LoadTexture(void* data, uint32_t width, uint32_t height, uint32_t channels)
	{
		uint32_t bytesPerPixel = GetFormatSize(_specs.Format);
		uint64_t imageSize = width * height * bytesPerPixel;
		CreateTexture();

		if (data) {
			memory::Scope<Buffer> stagingBuffer;
			stagingBuffer = memory::MakeScope<Buffer>(
				imageSize,
				vk::BufferUsageFlagBits::eTransferSrc,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
			);
			stagingBuffer->Map();
			stagingBuffer->Write(data);
			stagingBuffer->Unmap();
			_context.TransitionImageLayout(_image, _specs.Format, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
			_context.CopyBufferToImage(stagingBuffer->GetBuffer(), _image, width, height);
			_context.TransitionImageLayout(_image, _specs.Format, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
		}
		else {
			Logger_renderer::warn("Failed to load shader data is invalid");
		}
	}

	void Texture2D::CreateTexture()
	{
		vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
		if (_specs.Attachment) {
			if (IsDepthFormat(_specs.Format)) {
				usage |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
			}
			else {
				usage |= vk::ImageUsageFlagBits::eColorAttachment;
			}
		}
		_context.CreateImage(
			_specs.Width,
			_specs.Height,
			_specs.Format,
			vk::ImageTiling::eOptimal,
			usage,
			vk::MemoryPropertyFlagBits::eDeviceLocal,
			_image,
			_imageMemory
		);
		CreateTextureImageView();
		CreateTextureSampler();
	}

	void Texture2D::CreateTextureImageView()
	{
		vk::ImageViewCreateInfo viewCreateInfo{
			.image = _image,
			.viewType = vk::ImageViewType::e2D,
			.format = _specs.Format
		};
		viewCreateInfo.subresourceRange.aspectMask = TextureFormatToAspectFlags(_specs.Format);
		viewCreateInfo.subresourceRange.baseMipLevel = 0;
		viewCreateInfo.subresourceRange.levelCount = 1;
		viewCreateInfo.subresourceRange.baseArrayLayer = 0;
		viewCreateInfo.subresourceRange.layerCount = 1;
		_imageView = _context.GetDevice().createImageView(viewCreateInfo);
	}

	void Texture2D::CreateTextureSampler()
	{
		vk::SamplerCreateInfo samplerCreateInfo{
			.magFilter = _specs.Filter,
			.minFilter = _specs.Filter,
			.addressModeU = _specs.Wrap,
			.addressModeV = _specs.Wrap,
			.addressModeW = _specs.Wrap,
			.mipLodBias = 0.0f,
			.anisotropyEnable = false,
			.maxAnisotropy = _context.GetPhysicalDeviceProperties().limits.maxSamplerAnisotropy,
			.compareEnable = false,
			.compareOp = vk::CompareOp::eAlways,
			.minLod = 0.0f,
			.maxLod = 1.0f,
			.borderColor = vk::BorderColor::eIntOpaqueBlack,
			.unnormalizedCoordinates = false
		};
		_imageSampler = _context.GetDevice().createSampler(samplerCreateInfo);
	}

	Texture2D::Texture2D(const TextureSpecification& specs)
		: _context(Application::Get()->GetWindow().GetRenderContext())
	{
		_specs = specs;
	}

	Texture2D::Texture2D(const TextureSpecification& specs, const std::filesystem::path& path)
		: _context(Application::Get()->GetWindow().GetRenderContext())
	{
		_specs = specs;
		int width, height, channels;
		stbi_uc* data = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
		if (data) {
			LoadTexture(data, width, height, channels);
			stbi_image_free(data);
		}
		else {
			Logger_renderer::warn("Failed to load texture from: {}", path.string());
		}
	}

	Texture2D::Texture2D(const TextureSpecification& specs, DataBuffer data)
		: _context(Application::Get()->GetWindow().GetRenderContext())
	{
		_specs = specs;
		LoadTexture(data.Data, _specs.Width, _specs.Height, STBI_rgb_alpha);
		data.Release();
	}

	Texture2D::~Texture2D()
	{
		_context.GetDevice().destroySampler(_imageSampler);
		_context.GetDevice().destroyImageView(_imageView);
		_context.GetDevice().destroyImage(_image);
		_context.GetDevice().freeMemory(_imageMemory);
	}
}