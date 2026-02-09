#pragma once
#include "RenderContext.h"

namespace ae::grapichs {
	class Swapchain {
	public:
		Swapchain(RenderContext& context);
		~Swapchain();
		Swapchain(const Swapchain&) = delete;
		Swapchain& operator=(const Swapchain&) = delete;

		vk::Format GetSwapchainFormat() const { return _swapChainImageFormat; }
	private:
		void CreateSwapchain();
		void CreateSwapchainImageViews();
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

		RenderContext& _context;
	};
}