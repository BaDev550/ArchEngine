#include "ArchPch.h"
#include "Swapchain.h"
#include <GLFW/glfw3.h>
#include "ArchEngine/Core/Application.h"

namespace ae::grapichs {
	Swapchain::Swapchain(RenderContext& context)
		: _context(context)
	{
		PROFILE_SCOPE("Swapchain");
		try {
			CreateSwapchain();
			CreateSwapchainImageViews();
		}
		catch (const std::exception& e) {
			Logger_renderer::error("Failed to initialize swapchain: {}", e.what());
		}
	}

	Swapchain::~Swapchain() {
		for (auto& imageView : _swapChainImageViews)
			_context.GetDevice().destroyImageView(imageView);

		_context.GetDevice().destroySwapchainKHR(_swapChain);
	}

	void Swapchain::CreateSwapchain() {
		auto physicalDevice = _context.GetPhysicalDevice();
		auto surface = _context.GetSurface();

		QueueFamilyIndices indices = _context.FindPhysicalDeviceQueueFamilies();
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
		auto surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
		auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
		minImageCount = (surfaceCapabilities.maxImageCount > 0 && minImageCount > surfaceCapabilities.maxImageCount) ? surfaceCapabilities.maxImageCount : minImageCount;
		_swapChainSurfaceFormat = ChooseSwapSurfaceFormat(physicalDevice.getSurfaceFormatsKHR(surface));
		_swapChainExtent = ChooseSwapExtent(surfaceCapabilities);
		_swapChainImageFormat = _swapChainSurfaceFormat.format;

		vk::SwapchainCreateInfoKHR swapChainCreateInfo{
			.flags = vk::SwapchainCreateFlagsKHR(),
			.surface = surface,
			.minImageCount = minImageCount,
			.imageFormat = _swapChainSurfaceFormat.format,
			.imageColorSpace = _swapChainSurfaceFormat.colorSpace,
			.imageExtent = _swapChainExtent,
			.imageArrayLayers = 1,
			.imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
			.imageSharingMode = vk::SharingMode::eExclusive,
			.preTransform = surfaceCapabilities.currentTransform,
			.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
			.presentMode = ChooseSwapPresentMode(_context.GetPhysicalDevice().getSurfacePresentModesKHR(_context.GetSurface())),
			.clipped = true,
			.oldSwapchain = nullptr
		};
		if (!indices.IsSame()) {
			swapChainCreateInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
			swapChainCreateInfo.setQueueFamilyIndexCount(2);
			swapChainCreateInfo.setPQueueFamilyIndices(queueFamilyIndices);
		}
		else {
			swapChainCreateInfo.setImageSharingMode(vk::SharingMode::eExclusive);
		}
		_swapChain = _context.GetDevice().createSwapchainKHR(swapChainCreateInfo);
		_swapChainImages = _context.GetDevice().getSwapchainImagesKHR(_swapChain);
	}

	void Swapchain::CreateSwapchainImageViews() {
		_swapChainImageViews.clear();

		vk::ImageViewCreateInfo imageViewCreateInfo{
			.viewType = vk::ImageViewType::e2D,
			.format = _swapChainImageFormat,
			.subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }
		};

		for (auto image : _swapChainImages) {
			imageViewCreateInfo.setImage(image);
			_swapChainImageViews.emplace_back(_context.GetDevice().createImageView(imageViewCreateInfo));
		}
	}

	void Swapchain::CreateSyncObjects() {
		_imageAvailableSemaphores.resize(Renderer::MaxFramesInFlight);
		_renderFinishedSemaphores.resize(GetImageCount());
		_inFlightFences.resize(Renderer::MaxFramesInFlight);

		vk::SemaphoreCreateInfo createInfo{};
		vk::FenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;

		for (size_t i = 0; i < Renderer::MaxFramesInFlight; i++) {
			_imageAvailableSemaphores[i] = _context.GetDevice().createSemaphore(createInfo);
			_inFlightFences[i] = _context.GetDevice().createFence(fenceCreateInfo);
		}

		for (size_t i = 0; i < GetImageCount(); i++) {
			_renderFinishedSemaphores[i] = _context.GetDevice().createSemaphore(createInfo);
		}
	}

	void Swapchain::Submit(vk::CommandBuffer* cmd, uint32_t* imageIndex) {
		vk::SubmitInfo submitInfo{};
		vk::Semaphore waitSemaphore[] = { _imageAvailableSemaphores[Renderer::GetFrameIndex()]};
		vk::Semaphore signalSemaphore[] = { _renderFinishedSemaphores[*imageIndex] };
		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphore;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = cmd;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphore;
		_context.GetGrapichsQueue().submit(submitInfo);

		vk::PresentInfoKHR presentInfo{};
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = waitSemaphore;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &_swapChain;
		presentInfo.pImageIndices = imageIndex;
		CHECKF(_context.GetPresentQueue().presentKHR(presentInfo) == vk::Result::eSuccess, "Failed to swap buffers");
	}

	vk::Result Swapchain::Swapbuffers(uint32_t* imageIndex) {
		uint32_t frameIndex = Renderer::GetFrameIndex();
		vk::Result result = _context.GetDevice().waitForFences(1, &_inFlightFences[frameIndex], vk::True, UINT32_MAX);
		CHECKF(result == vk::Result::eSuccess, "Failed to wait for events");

		result = _context.GetDevice().acquireNextImageKHR(_swapChain, UINT32_MAX, _imageAvailableSemaphores[*imageIndex], nullptr, imageIndex);
		CHECKF(result == vk::Result::eSuccess, "Failed to quire next image");
		CHECKF(_context.GetDevice().resetFences(1, &_inFlightFences[frameIndex]) == vk::Result::eSuccess, "Failed to reset fence");
		return result;
	}

	vk::SurfaceFormatKHR Swapchain::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
				return availableFormat;
		}
		return availableFormats[0];
	}

	vk::PresentModeKHR Swapchain::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
				return availablePresentMode;
			}
		}
		return vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D Swapchain::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != UINT32_MAX) {
			return capabilities.currentExtent;
		}
		int width, height;
		glfwGetFramebufferSize(Application::Get()->GetWindow().GetHandle(), &width, &height);

		return {
			std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
			std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
		};
	}
}