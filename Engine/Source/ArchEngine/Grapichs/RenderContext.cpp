#include "ArchPch.h"
#include "RenderContext.h"

#include <set>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace ae::grapichs {
	//static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
	//	vk::DebugUtilsMessageSeverityFlagBitsEXT severity, 
	//	vk::DebugUtilsMessageTypeFlagsEXT type, 
	//	const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*
	//) {
	//	if (severity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
	//		Logger_renderer::error("Validation layer: {}", pCallbackData->pMessage);
	//	return vk::False;
	//}

	RenderContext::RenderContext()
	{
		PROFILE_SCOPE("Render Context");
		try {
			CreateInstance();
			//CreateDebugMessenger();
			PickPhysicalDevice();
			CreateLogicalDevice();
		}
		catch (const std::exception& e) {
			Logger_renderer::error("Render contex failed to initialize: {}", e.what());
			throw;
		}
		Logger_renderer::info("Render context created!");
	}

	RenderContext::~RenderContext()
	{
		_logicalDevice.destroy();
		_instance.destroy();
	}

	void RenderContext::WaitDeviceIdle()
	{
		vkDeviceWaitIdle(_logicalDevice);
	}

	void RenderContext::CreateInstance()
	{
		if (_enableValidationLayer && !CheckValidationLayerSupport()) {
			throw std::runtime_error("Validation layers are requested but not available!!!");
		}

		std::vector<const char*> extensions = GetRequiredExtensions();

		const vk::ApplicationInfo appInfo{
			.pEngineName = "ArchEngine",
			.apiVersion = vk::ApiVersion13
		};

		const vk::InstanceCreateInfo createInfo{
			.pNext = nullptr,
			.pApplicationInfo = &appInfo,
			.enabledLayerCount = static_cast<uint32_t>(_layers.size()),
			.ppEnabledLayerNames = _layers.data(),
			.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
			.ppEnabledExtensionNames = extensions.data()
		};

		_instance = vk::createInstance(createInfo);
		CHECKF(_instance != VK_NULL_HANDLE, "Failed to create vulkan instance");
	}

	void RenderContext::CreateDebugMessenger()
	{
		if (!_enableValidationLayer) return;
	}

	void RenderContext::PickPhysicalDevice()
	{
		uint32_t physicalDeviceCount;
		_instance.enumeratePhysicalDevices(&physicalDeviceCount, nullptr);
		std::vector<vk::PhysicalDevice> physicsDevices(physicalDeviceCount);
		_instance.enumeratePhysicalDevices(&physicalDeviceCount, physicsDevices.data());

		for (const auto& physicalDevice : physicsDevices) {
			if (IsPhysicalDeviceSuitable(physicalDevice)) {
				_physicalDevice = physicalDevice;
				break;
			}
		}
		CHECKF(_physicalDevice != VK_NULL_HANDLE, "Failed to find suitable GPU");
		_physicalDevice.getProperties(&_physicalDeviceProperties);

		Logger_renderer::info("Selected GPU Information: ");
		Logger_renderer::info("	GPU Name: {}", (const char*)_physicalDeviceProperties.deviceName);
	}

	void RenderContext::CreateLogicalDevice()
	{
		QueueFamilyIndices indices = FindQueueFamilies(_physicalDevice);
		float queuePriority = 1.0f;

		const vk::DeviceQueueCreateInfo queueCreateInfo{
			.queueFamilyIndex = indices.graphicsFamily.value(),
			.queueCount = 1,
			.pQueuePriorities = &queuePriority
		};

		const vk::PhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{
			.dynamicRendering = VK_TRUE
		};
		
		const vk::DeviceCreateInfo createInfo{
			.pNext = &dynamicRenderingFeatures,
			.queueCreateInfoCount = 1,
			.pQueueCreateInfos = &queueCreateInfo,
			.enabledLayerCount = static_cast<uint32_t>(_layers.size()),
			.ppEnabledLayerNames = _layers.data(),
			.enabledExtensionCount = static_cast<uint32_t>(_extensions.size()),
			.ppEnabledExtensionNames = _extensions.data(),
		};
		
		_logicalDevice = _physicalDevice.createDevice(createInfo);
		_grapichsQueue = _logicalDevice.getQueue(indices.graphicsFamily.value(), 0);
	}

	bool RenderContext::IsPhysicalDeviceSuitable(vk::PhysicalDevice device)
	{
		QueueFamilyIndices indices = FindQueueFamilies(device);
		bool extensionSupported = HasRequiredDeviceExtensions(device);
		return extensionSupported && indices.IsComplete();
	}

	bool RenderContext::HasRequiredDeviceExtensions(vk::PhysicalDevice device)
	{
		uint32_t exensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &exensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(exensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &exensionCount, availableExtensions.data());
		std::set<std::string> requiredExtensions(_extensions.begin(), _extensions.end());
		for (const auto& extension : availableExtensions)
			requiredExtensions.erase(extension.extensionName);
		return requiredExtensions.empty();
	}

	bool RenderContext::CheckValidationLayerSupport()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : _layers) {
			bool layerFound = false;
			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}
			if (!layerFound)
				return false;
		}
		return true;
	}

	std::vector<const char*> RenderContext::GetRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
		if (_enableValidationLayer)
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		return extensions;
	}

	QueueFamilyIndices RenderContext::FindQueueFamilies(vk::PhysicalDevice device)
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
		
		for (int i = 0; i < queueFamilyCount; i++) {
			if (indices.IsComplete())
				break;

			const auto& queueFamily = queueFamilies[i];
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.graphicsFamily = i;
		}
		return indices;
	}
}