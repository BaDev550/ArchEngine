#include "ArchPch.h"
#include "RenderPass.h"
#include "Renderer.h"

namespace ae::grapichs {
	namespace Utils {
        bool hasStencilComponent(vk::Format format) {
            return format == vk::Format::eD32SfloatS8Uint ||
                format == vk::Format::eD24UnormS8Uint ||
                format == vk::Format::eD16UnormS8Uint ||
                format == vk::Format::eS8Uint;
        }

		// Copied from the "3D Graphics Rendering Cookbook"
		void ImageMemBarrier(
			vk::CommandBuffer CmdBuf, 
			vk::Image Image, 
			vk::Format Format, 
			vk::ImageLayout OldLayout, 
			vk::ImageLayout NewLayout,
			int LayerCount
		)
		{
            vk::ImageMemoryBarrier barrier;
            barrier.oldLayout = OldLayout;
            barrier.newLayout = NewLayout;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = Image;

            // Default subresource range
            barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = LayerCount;

            vk::PipelineStageFlags sourceStage = vk::PipelineStageFlagBits::eNone;
            vk::PipelineStageFlags destinationStage = vk::PipelineStageFlagBits::eNone;

            // Detect Depth/Stencil Formats
            if (NewLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal ||
                Format == vk::Format::eD16Unorm ||
                Format == vk::Format::eX8D24UnormPack32 ||
                Format == vk::Format::eD32Sfloat ||
                Format == vk::Format::eS8Uint ||
                Format == vk::Format::eD16UnormS8Uint ||
                Format == vk::Format::eD24UnormS8Uint)
            {
                barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;

                if (hasStencilComponent(Format)) {
                    barrier.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
                }
            }
            else {
                barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            }

            // Layout Transition Logic
            if (OldLayout == vk::ImageLayout::eUndefined && NewLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
                barrier.srcAccessMask = vk::AccessFlags(); // 0
                barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

                sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
                destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
            }
            else if (OldLayout == vk::ImageLayout::eUndefined && NewLayout == vk::ImageLayout::eGeneral) {
                barrier.srcAccessMask = vk::AccessFlags();
                barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

                sourceStage = vk::PipelineStageFlagBits::eTransfer;
                destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
            }
            else if (OldLayout == vk::ImageLayout::eUndefined && NewLayout == vk::ImageLayout::eTransferDstOptimal) {
                barrier.srcAccessMask = vk::AccessFlags();
                barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

                sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
                destinationStage = vk::PipelineStageFlagBits::eTransfer;
            }
            // Convert back from read-only to updateable
            else if (OldLayout == vk::ImageLayout::eShaderReadOnlyOptimal && NewLayout == vk::ImageLayout::eTransferDstOptimal) {
                barrier.srcAccessMask = vk::AccessFlagBits::eShaderRead;
                barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

                sourceStage = vk::PipelineStageFlagBits::eFragmentShader;
                destinationStage = vk::PipelineStageFlagBits::eTransfer;
            }
            // Convert from updateable texture to shader read-only
            else if (OldLayout == vk::ImageLayout::eTransferDstOptimal && NewLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
                barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
                barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

                sourceStage = vk::PipelineStageFlagBits::eTransfer;
                destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
            }
            // Convert depth texture from undefined state to depth-stencil buffer
            else if (OldLayout == vk::ImageLayout::eUndefined && NewLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
                barrier.srcAccessMask = vk::AccessFlags();
                barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

                sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
                destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
            }
            // Wait for render pass to complete
            else if (OldLayout == vk::ImageLayout::eShaderReadOnlyOptimal && NewLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
                barrier.srcAccessMask = vk::AccessFlags();
                barrier.dstAccessMask = vk::AccessFlags();

                sourceStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
                destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
            }
            // Convert back from read-only to color attachment
            else if (OldLayout == vk::ImageLayout::eShaderReadOnlyOptimal && NewLayout == vk::ImageLayout::eColorAttachmentOptimal) {
                barrier.srcAccessMask = vk::AccessFlagBits::eShaderRead;
                barrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

                sourceStage = vk::PipelineStageFlagBits::eFragmentShader;
                destinationStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            }
            // Convert from updateable texture to shader read-only
            else if (OldLayout == vk::ImageLayout::eColorAttachmentOptimal && NewLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
                barrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
                barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eInputAttachmentRead;

                sourceStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
                destinationStage = vk::PipelineStageFlagBits::eAllGraphics; // Or eFragmentShader depending on needs
            }
            // Convert back from read-only to depth attachment
            else if (OldLayout == vk::ImageLayout::eShaderReadOnlyOptimal && NewLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
                barrier.srcAccessMask = vk::AccessFlagBits::eShaderRead;
                barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;

                sourceStage = vk::PipelineStageFlagBits::eFragmentShader;
                destinationStage = vk::PipelineStageFlagBits::eLateFragmentTests;
            }
            // Convert from updateable depth texture to shader read-only
            else if (OldLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal && NewLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
                barrier.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
                barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

                sourceStage = vk::PipelineStageFlagBits::eLateFragmentTests;
                destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
            }
            else if (OldLayout == vk::ImageLayout::eUndefined && NewLayout == vk::ImageLayout::eColorAttachmentOptimal) {
                barrier.srcAccessMask = vk::AccessFlags();
                barrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

                sourceStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
                destinationStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            }
            else if (OldLayout == vk::ImageLayout::eColorAttachmentOptimal && NewLayout == vk::ImageLayout::ePresentSrcKHR) {
                barrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
                barrier.dstAccessMask = vk::AccessFlags();

                sourceStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
                destinationStage = vk::PipelineStageFlagBits::eBottomOfPipe;
            }
            else {
                throw std::invalid_argument("Unknown layout transition in ImageMemBarrier");
            }

            CmdBuf.pipelineBarrier(
                sourceStage,
                destinationStage,
                vk::DependencyFlags(),
                nullptr,
                nullptr,
                barrier
            );
		}
	}
	RenderPass::RenderPass(memory::Ref<Pipeline>& pipeline) {
		_descriptorManager = memory::Ref<DescriptorManager>::Create(pipeline->GetShader());
	}

	RenderPass::~RenderPass() {
		_descriptorManager = nullptr;
	}

	void RenderPass::Begin() {
		vk::CommandBuffer cmd = Renderer::GetCurrentCommandBuffer();
		PipelineData pipelineData = _pipeline->GetPipelineData();
		uint32_t frameIndex = Renderer::GetFrameIndex();
		auto& framebuffer = pipelineData.TargetFramebuffer;
		auto& shader = pipelineData.Shader;
		const FramebufferSpecification fbSpecs = framebuffer->GetSpecification();
		const uint32_t attachmentCount = framebuffer->GetAttachmentCount();
		const glm::vec4 clearColor = fbSpecs.ClearColor;

		std::vector<vk::RenderingAttachmentInfo> colorAttachments(attachmentCount);
		vk::RenderingAttachmentInfo depthAttachments{};
		vk::Extent2D extent{ framebuffer->GetWidth(), framebuffer->GetHeight() };
		bool hasDepthBuffer = framebuffer->DoesFramebufferHasDepthAttachment();

		if (hasDepthBuffer) {
			const float depthClearColor = fbSpecs.DepthClearValue;
			const memory::Ref<Texture2D>& depthAttachment = framebuffer->GetDepthTexture();
			vk::Image fbDepthImage = depthAttachment->GetImage();
			vk::Format fbDepthFormat = depthAttachment->GetFormat();
			depthAttachments.imageView = depthAttachment->GetImageView();
			depthAttachments.imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
			depthAttachments.loadOp = vk::AttachmentLoadOp::eClear;
			depthAttachments.storeOp = vk::AttachmentStoreOp::eStore;
			depthAttachments.clearValue = vk::ClearValue(depthClearColor);
            Utils::ImageMemBarrier(cmd, fbDepthImage, fbDepthFormat, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal, 1);
		}

		for (int i = 0; i < colorAttachments.size(); i++) {
			const memory::Ref<Texture2D>& colorAttachment = framebuffer->GetAttachmentTexture(i);
			vk::Image fbImage = colorAttachment->GetImage();
			vk::Format fbFormat = colorAttachment->GetFormat();
			colorAttachments[i].imageView = colorAttachment->GetImageView();
			colorAttachments[i].imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
			colorAttachments[i].loadOp = vk::AttachmentLoadOp::eClear;
			colorAttachments[i].storeOp = vk::AttachmentStoreOp::eStore;
			colorAttachments[i].clearValue = vk::ClearValue({ clearColor.x, clearColor.g, clearColor.b, clearColor.a });
            Utils::ImageMemBarrier(cmd, fbImage, fbFormat, vk::ImageLayout::eUndefined, vk::ImageLayout::eAttachmentOptimal, 1);
		}

		vk::RenderingInfo renderingInfo{};
		renderingInfo.renderArea = vk::Rect2D({0, 0}, extent);
		renderingInfo.layerCount = 1;
		renderingInfo.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
		renderingInfo.pColorAttachments = colorAttachments.data();
		renderingInfo.pDepthAttachment = &depthAttachments;

		vk::Viewport viewport{ 0, 0, (float)extent.width, (float)extent.height };
		vk::Rect2D scissor{ {0, 0}, extent };
		viewport.minDepth = 0;
		viewport.maxDepth = 1;
		cmd.beginRendering(renderingInfo);
		cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, _pipeline->GetPipeline());
		cmd.setViewport(0, 1, &viewport);
		cmd.setScissor(0, 1, &scissor);
	}

	void RenderPass::End() {
		vk::CommandBuffer cmd = Renderer::GetCurrentCommandBuffer();
		cmd.endRendering(cmd);
	}

	void RenderPass::SetInput(std::string_view name, const memory::Ref<Buffer>& buffer) {
		_descriptorManager->WriteInput(name, buffer);
	}
	void RenderPass::SetInput(std::string_view name, const memory::Ref<Texture2D>& texture) {
		_descriptorManager->WriteInput(name, texture);
	}
}