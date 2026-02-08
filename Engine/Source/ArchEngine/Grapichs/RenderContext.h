#pragma once
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <vector>
#include <optional>

#define AE_GRAPICHS_DEBUG_VULKAN_VALIDATION_LAYER true

namespace ae {
	class Window;
}

namespace ae::grapichs {
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		bool IsComplete() const { return graphicsFamily.has_value() && presentFamily.has_value(); }
		bool IsSame() const { return graphicsFamily.value() == presentFamily.value(); }
	};

	class RenderContext {
	public:
		RenderContext(Window* window);
		RenderContext(const RenderContext&) = delete;
		RenderContext& operator=(const RenderContext&) = delete;
		~RenderContext();

		QueueFamilyIndices FindPhysicalDeviceQueueFamilies() { return FindQueueFamilies(_physicalDevice); }

		void WaitDeviceIdle();

		vk::Instance GetInstance() const { return _instance; }
		vk::Device GetDevice() const { return _logicalDevice; }
		vk::PhysicalDeviceProperties GetPhysicalDeviceProperties() { return _physicalDeviceProperties; }
		vk::PhysicalDevice GetPhysicalDevice() const { return _physicalDevice; }
		vk::SurfaceKHR GetSurface() const { return _surface; }
		vk::Queue GetGrapichsQueue() const { return _grapichsQueue; }
		vk::Queue GetPresentQueue() const { return _presentQueue; }
	private:
		void CreateInstance();
		void CreateDebugMessenger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		bool IsPhysicalDeviceSuitable(vk::PhysicalDevice device);
		bool HasRequiredDeviceExtensions(vk::PhysicalDevice device);
		bool CheckValidationLayerSupport();
		std::vector<const char*> GetRequiredExtensions();
		QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device);
	private:
		vk::Instance _instance = VK_NULL_HANDLE;
		vk::PhysicalDeviceProperties _physicalDeviceProperties;
		vk::PhysicalDevice _physicalDevice = VK_NULL_HANDLE;
		vk::Device _logicalDevice = VK_NULL_HANDLE;
		vk::SurfaceKHR _surface = VK_NULL_HANDLE;

		Window& _window;
		vk::Queue _grapichsQueue = VK_NULL_HANDLE;
		vk::Queue _presentQueue = VK_NULL_HANDLE;
		vk::DebugUtilsMessengerEXT _debugMessanger;
		
		const std::vector<const char*> _extensions = { vk::KHRSwapchainExtensionName };
		const std::vector<const char*> _layers = { "VK_LAYER_KHRONOS_validation" };
#if AE_GRAPICHS_DEBUG_VULKAN_VALIDATION_LAYER
		const bool _enableValidationLayer = true;
#else
		const bool _enableValidationLayer = false;
#endif
	};
}