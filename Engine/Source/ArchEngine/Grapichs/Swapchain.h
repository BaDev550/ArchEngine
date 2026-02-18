#pragma once
#include "RenderContext.h"

namespace ae::grapichs {
	class Swapchain {
	public:
		Swapchain(RenderContext& context);
		~Swapchain();
		Swapchain(const Swapchain&) = delete;
		Swapchain& operator=(const Swapchain&) = delete;
		size_t GetImageCount() const { return _swapChainImages.size(); }

		vk::Result Submit(vk::CommandBuffer* cmd, uint32_t* imageIndex);
		vk::Result Swapbuffers(uint32_t* imageIndex);
		vk::Format GetSwapchainFormat() const { return _swapChainImageFormat; }
		vk::Format GetSwapchainDepthFormat() const { return _swapChainDepthFormat; }
	private:
		void CreateSwapchain();
		void CreateSwapchainImageViews();
		void CreateDepthResources();
		void CreateSyncObjects();
		vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
		vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
		vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
		vk::Format FindDepthFormat();
	private:
		vk::SwapchainKHR _swapChain;
		vk::Format _swapChainImageFormat = vk::Format::eUndefined;
		vk::Extent2D _swapChainExtent;
		vk::SurfaceFormatKHR _swapChainSurfaceFormat;

		std::vector<vk::Image> _swapChainImages;
		std::vector<vk::ImageView> _swapChainImageViews;
		std::vector<vk::Semaphore> _imageAvailableSemaphores;
		std::vector<vk::Semaphore> _renderFinishedSemaphores;
		std::vector<vk::Fence> _inFlightFences;

		vk::Image _swapChainDepthImage;
		vk::Format _swapChainDepthFormat;
		vk::DeviceMemory _swapChainDepthImageMemory;
		vk::ImageView _swapChainDepthImageView;

		RenderContext& _context;
		friend class Framebuffer;
	};
}