#include "ArchPch.h"
#include "Framebuffer.h"
#include "ArchEngine/Core/Application.h"

namespace ae::grapichs {
	Framebuffer::Framebuffer(const FramebufferSpecification& spec)
		: _specs(spec)
	{
		if (_specs.Width == 0 || _specs.Height == 0) {
			_specs.Width = Application::Get()->GetWindow().GetWidth();
			_specs.Height = Application::Get()->GetWindow().GetHeight();
		}
		if (_specs.IsSwapchain) {
			auto& swapchain = Application::Get()->GetWindow().GetSwapchain();
			for (size_t i = 0; i < swapchain._swapChainImages.size(); i++) {
				vk::Image swapchainImage = swapchain._swapChainImages[i];
				vk::ImageView swapchainImageView = swapchain._swapChainImageViews[i];
				TextureSpecification colorAttachment{};
				colorAttachment.Format = swapchain.GetSwapchainFormat();
				colorAttachment.Attachment = true;
				auto swapchainTexture = memory::Ref<Texture2D>::Create(colorAttachment, swapchainImage, swapchainImageView);
				_colorAttachments.emplace_back(std::move(swapchainTexture));
			}
			TextureSpecification depthAttachment{};
			depthAttachment.Format = swapchain.GetSwapchainDepthFormat();
			depthAttachment.Attachment = true;
			auto swapchainDepthTexture = memory::Ref<Texture2D>::Create(depthAttachment, swapchain._swapChainDepthImage, swapchain._swapChainDepthImageView);
			_depthAttachment = std::move(swapchainDepthTexture);
		}
		else {
			for (const auto& attachment : _specs.Attachments.Attachments) {
				TextureSpecification spec{};
				spec.Format = attachment;
				spec.Width = _specs.Width;
				spec.Height = _specs.Height;
				spec.Attachment = true;
				if (IsDepthFormat(attachment)) {
					_depthAttachment = memory::Ref<Texture2D>::Create(spec);
				}
				else {
					_colorAttachments.emplace_back(memory::Ref<Texture2D>::Create(spec));
				}
			}
		}
	}
	Framebuffer::~Framebuffer() {}

	void Framebuffer::Resize(uint32_t width, uint32_t height) {
		
	}
}