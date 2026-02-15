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

		void Submit(vk::CommandBuffer* cmd, uint32_t* imageIndex);
		vk::Result Swapbuffers(uint32_t* imageIndex);
		vk::Format GetSwapchainFormat() const { return _swapChainImageFormat; }
	private:
		void CreateSwapchain();
		void CreateSwapchainImageViews();
		void CreateSyncObjects();
		vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
		vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
		vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
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

		RenderContext& _context;
		friend class Framebuffer;
	};
}