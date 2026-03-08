#include "ArchPch.h"
#include "TextureCube.h"
#include "ArchEngine/Core/Application.h"

namespace ae::grapichs {
    TextureCube::TextureCube(const TextureSpecification& specs, const std::vector<Bitmap>& faces)
        : _context(Application::Get()->GetWindow().GetRenderContext())
    {
        _specs = specs;
        if (faces.size() != 6) {
            Logger_renderer::error("TextureCube requires exactly 6 faces!");
            return;
        }

        _specs.Width = faces[0]._w;
        _specs.Height = faces[0]._h;

        uint32_t bytesPerPixel = faces[0]._comp * GetBytesPerComponent(faces[0].fmt_);
        uint64_t layerSize = _specs.Width * _specs.Height * bytesPerPixel;
        uint64_t totalImageSize = layerSize * 6;

        CreateTexture();

        memory::Scope<Buffer> stagingBuffer = memory::MakeScope<Buffer>(
            totalImageSize,
            vk::BufferUsageFlagBits::eTransferSrc,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
        );
        stagingBuffer->Map();
        for (int i = 0; i < 6; i++) { stagingBuffer->Write(faces[i]._data.Data, layerSize, i * layerSize); }
        stagingBuffer->Unmap();

        _context.TransitionImageLayout(_image, _specs.Format, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, 6);
        _context.CopyBufferToImage(stagingBuffer->GetBuffer(), _image, _specs.Width, _specs.Height, 6);
        _context.TransitionImageLayout(_image, _specs.Format, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, 6);
    }

    TextureCube::~TextureCube()
    {
        if (_imageSampler)
            _context.GetDevice().destroySampler(_imageSampler);
        _context.GetDevice().destroyImageView(_imageView);
        _context.GetDevice().destroyImage(_image);
        _context.GetDevice().freeMemory(_imageMemory);
    }

    void TextureCube::CreateTexture() {
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
            _imageMemory,
            6, 
            vk::ImageCreateFlagBits::eCubeCompatible
        );
        CreateTextureImageView();
        CreateTextureSampler();
        _imageInfo = {
                .sampler = _imageSampler,
                .imageView = _imageView,
                .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal
        };
    }

    void TextureCube::CreateTextureImageView() {
        vk::ImageViewCreateInfo viewCreateInfo{
            .image = _image,
            .viewType = vk::ImageViewType::eCube,
            .format = _specs.Format
        };
        viewCreateInfo.subresourceRange.aspectMask = TextureFormatToAspectFlags(_specs.Format);
        viewCreateInfo.subresourceRange.baseMipLevel = 0;
        viewCreateInfo.subresourceRange.levelCount = 1;
        viewCreateInfo.subresourceRange.baseArrayLayer = 0;
        viewCreateInfo.subresourceRange.layerCount = 6;
        _imageView = _context.GetDevice().createImageView(viewCreateInfo);
    }

    void TextureCube::CreateTextureSampler() {
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
}