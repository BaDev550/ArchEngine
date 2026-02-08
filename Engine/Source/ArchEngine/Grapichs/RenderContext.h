#pragma once
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vector>
#include <optional>

#define AE_GRAPICHS_DEBUG_VULKAN_VALIDATION_LAYER true

namespace ae::grapichs {
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		bool IsComplete() const { return graphicsFamily.has_value(); }
	};

	class RenderContext {
	public:
		RenderContext();
		RenderContext(const RenderContext&) = delete;
		RenderContext& operator=(const RenderContext&) = delete;
		~RenderContext();

		void WaitDeviceIdle();

		vk::Instance GetInstance() const { return _instance; }
		vk::Device GetDevice() const { return _logicalDevice; }
		vk::PhysicalDeviceProperties GetPhysicalDeviceProperties() { return _physicalDeviceProperties; }
		vk::PhysicalDevice GetPhysicalDevice() const { return _physicalDevice; }
		vk::Queue GetGrapichsQueue() const { return _grapichsQueue; }
	private:
		void CreateInstance();
		void CreateDebugMessenger();
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

		vk::Queue _grapichsQueue = VK_NULL_HANDLE;
		vk::DebugUtilsMessengerEXT _debugMessanger;

		const std::vector<const char*> _extensions;
		const std::vector<const char*> _layers = { "VK_LAYER_KHRONOS_validation" };
#if AE_GRAPICHS_DEBUG_VULKAN_VALIDATION_LAYER
		const bool _enableValidationLayer = true;
#else
		const bool _enableValidationLayer = false;
#endif
	};
}