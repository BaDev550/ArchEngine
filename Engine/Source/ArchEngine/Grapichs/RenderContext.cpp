#include "ArchPch.h"
#include "RenderContext.h"

#include "ArchEngine/Core/Window.h"

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define NOMINMAX
#include <GLFW/glfw3native.h>
#include <set>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace ae::grapichs {
	void PipelineConfig::Default(PipelineConfig& config) {
		config.InputAssembyCreateInfo.topology = vk::PrimitiveTopology::eTriangleList;
		config.InputAssembyCreateInfo.primitiveRestartEnable = VK_FALSE;

		config.ViewportStateCreateInfo.viewportCount = 1;
		config.ViewportStateCreateInfo.scissorCount = 1;
		config.ViewportStateCreateInfo.pViewports = nullptr;
		config.ViewportStateCreateInfo.pScissors = nullptr;

		config.ResterizationStateCreateInfo = {};
		config.ResterizationStateCreateInfo.depthClampEnable = VK_FALSE;
		config.ResterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		config.ResterizationStateCreateInfo.polygonMode = vk::PolygonMode::eFill;
		config.ResterizationStateCreateInfo.cullMode =	  vk::CullModeFlagBits::eBack;
		config.ResterizationStateCreateInfo.frontFace =	  vk::FrontFace::eClockwise;
		config.ResterizationStateCreateInfo.lineWidth = 1.0f;
		config.ResterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
		config.ResterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
		config.ResterizationStateCreateInfo.depthBiasClamp = 0.0f;
		config.ResterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;

		config.MultisampleStateCreateInfo = {};
		config.MultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
		config.MultisampleStateCreateInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
		config.MultisampleStateCreateInfo.minSampleShading = 1.0f;
		config.MultisampleStateCreateInfo.pSampleMask = nullptr;
		config.MultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
		config.MultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

		config.ColorBlendAttachment = {};
		config.ColorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
		config.ColorBlendAttachment.blendEnable = VK_FALSE;
		config.ColorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne;
		config.ColorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero;
		config.ColorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
		config.ColorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
		config.ColorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
		config.ColorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

		config.ColorBlendStateCreateInfo = {};
		config.ColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
		config.ColorBlendStateCreateInfo.logicOp = vk::LogicOp::eCopy;
		config.ColorBlendStateCreateInfo.attachmentCount = 1;
		config.ColorBlendStateCreateInfo.pAttachments = &config.ColorBlendAttachment;
		config.ColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
		config.ColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
		config.ColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
		config.ColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

		config.DepthStencilCreateInfo = {};
		config.DepthStencilCreateInfo.depthTestEnable = VK_TRUE;
		config.DepthStencilCreateInfo.depthWriteEnable = VK_TRUE;
		config.DepthStencilCreateInfo.depthCompareOp = vk::CompareOp::eLess;
		config.DepthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
		config.DepthStencilCreateInfo.minDepthBounds = 0.0f;
		config.DepthStencilCreateInfo.maxDepthBounds = 1.0f;
		config.DepthStencilCreateInfo.stencilTestEnable = VK_FALSE;
		config.DepthStencilCreateInfo.front = {};
		config.DepthStencilCreateInfo.back = {};

		config.DynamicStateEnables = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
		config.DynamicStateCreateInfo.pDynamicStates = config.DynamicStateEnables.data();
		config.DynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(config.DynamicStateEnables.size());
	}

	static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
		vk::DebugUtilsMessageSeverityFlagBitsEXT severity, 
		vk::DebugUtilsMessageTypeFlagsEXT type, 
		const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*
	) {
		if (severity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
			Logger_renderer::error("Validation layer: {}", pCallbackData->pMessage);
		return vk::False;
	}

	RenderContext::RenderContext(Window* window)
		: _window(*window)
	{
		PROFILE_SCOPE("Render Context");
		try {
			CreateInstance();
			CreateDebugMessenger();
			CreateSurface();
			PickPhysicalDevice();
			CreateLogicalDevice();
			CreateContextCommandPool();
		}
		catch (const std::exception& e) {
			Logger_renderer::error("Render contex failed to initialize: {}", e.what());
			throw;
		}
		Logger_renderer::info("Render context created!");
	}

	RenderContext::~RenderContext()
	{
		if (_enableValidationLayer) _instance.destroyDebugUtilsMessengerEXT(_debugMessanger);

		_logicalDevice.destroy();
		_instance.destroySurfaceKHR(_surface);
		_instance.destroy();
	}

	void RenderContext::WaitDeviceIdle()
	{
		if (_logicalDevice)
			_logicalDevice.waitIdle();
	}

	void RenderContext::CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memoryProperties, vk::Buffer& buffer, vk::DeviceMemory& memory)
	{
		const vk::BufferCreateInfo bufferInfo{
			.size = size,
			.usage = usage,
			.sharingMode = vk::SharingMode::eExclusive
		};
		buffer = _logicalDevice.createBuffer(bufferInfo);
		CHECKF(buffer, "Failed to create buffer");

		vk::MemoryRequirements memRequirements;
		_logicalDevice.getBufferMemoryRequirements(buffer, &memRequirements);

		const vk::MemoryAllocateInfo allocInfo{
			.allocationSize = memRequirements.size,
			.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, memoryProperties)
		};
		memory = _logicalDevice.allocateMemory(allocInfo);
		CHECKF(memory, "Failed to allocate memory");
		_logicalDevice.bindBufferMemory(buffer, memory, 0);
	}

	void RenderContext::CreateImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags memoryProperties, vk::Image& image, vk::DeviceMemory& memory) {
		vk::ImageCreateInfo createInfo{};
		createInfo.imageType = vk::ImageType::e2D;
		createInfo.extent.width = width;
		createInfo.extent.height = height;
		createInfo.extent.depth = 1;
		createInfo.mipLevels = 1;
		createInfo.arrayLayers = 1;
		createInfo.tiling = tiling;
		createInfo.initialLayout = vk::ImageLayout::eUndefined;
		createInfo.usage = usage;
		createInfo.sharingMode = vk::SharingMode::eExclusive;
		createInfo.samples = vk::SampleCountFlagBits::e1;
		image = _logicalDevice.createImage(createInfo);
		CHECKF(image, "Failed to create image");

		vk::MemoryRequirements memRequirements;
		_logicalDevice.getImageMemoryRequirements(image, &memRequirements);

		const vk::MemoryAllocateInfo allocInfo{
			.allocationSize = memRequirements.size,
			.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, memoryProperties)
		};
		memory = _logicalDevice.allocateMemory(allocInfo);
		CHECKF(memory, "Failed to allocate memory");
		_logicalDevice.bindImageMemory(image, memory, 0);
	}

	void RenderContext::CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size) {
		vk::CommandBuffer cmd = BeginSingleTimeCommand();
		vk::BufferCopy copyRegion{
			.srcOffset = 0,
			.dstOffset = 0,
			.size = size,
		};
		cmd.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);
		EndSingleTimeCommand(cmd);
	}

	void RenderContext::CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height) {
		vk::CommandBuffer cmd = BeginSingleTimeCommand();
		vk::BufferImageCopy region{
			.bufferOffset = 0,
			.bufferRowLength = 0,
			.bufferImageHeight = 0
		};
		region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { width, height, 1 };
		cmd.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);
		EndSingleTimeCommand(cmd);
	}

	void RenderContext::TransitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) {
		vk::CommandBuffer cmd = BeginSingleTimeCommand();
		vk::ImageMemoryBarrier barrier{};
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		vk::PipelineStageFlags sourceStage;
		vk::PipelineStageFlags dstStage;
		if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
			barrier.srcAccessMask = vk::AccessFlagBits::eNone;
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
			sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
			dstStage = vk::PipelineStageFlagBits::eTransfer;
		}
		else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
			sourceStage = vk::PipelineStageFlagBits::eTransfer;
			dstStage = vk::PipelineStageFlagBits::eFragmentShader;
		}
		else {
			CHECKF(false, "unsupported layout transform");
		}
		cmd.pipelineBarrier(
			sourceStage, dstStage,
			vk::DependencyFlags{},
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
		EndSingleTimeCommand(cmd);
	}

	void RenderContext::CreateInstance()
	{
		VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
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
		VULKAN_HPP_DEFAULT_DISPATCHER.init(_instance);
	}

	void RenderContext::CreateDebugMessenger()
	{
		if (!_enableValidationLayer) return;
		vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
		vk::DebugUtilsMessageTypeFlagsEXT    messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
		vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT{
			.messageSeverity = severityFlags,
			.messageType = messageTypeFlags,
			.pfnUserCallback = &debugCallback
		};
		_debugMessanger = _instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
	}

	void RenderContext::CreateSurface() {
		VkSurfaceKHR surface;
		if (glfwCreateWindowSurface(_instance, _window.GetHandle(), nullptr, &surface) != 0)
			throw std::runtime_error("Failed to create window surface");
		_surface = vk::SurfaceKHR(surface);
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
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqeQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
		for (uint32_t queueFamily : uniqeQueueFamilies) {
			vk::DeviceQueueCreateInfo queueCreateInfo{
				.queueFamilyIndex = indices.graphicsFamily.value(),
				.queueCount = 1,
				.pQueuePriorities = &queuePriority
			};
			queueCreateInfos.push_back(queueCreateInfo);
		};

		const vk::PhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{
			.dynamicRendering = VK_TRUE
		};
		
		const vk::DeviceCreateInfo createInfo{
			.pNext = &dynamicRenderingFeatures,
			.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
			.pQueueCreateInfos = queueCreateInfos.data(),
			.enabledLayerCount = static_cast<uint32_t>(_layers.size()),
			.ppEnabledLayerNames = _layers.data(),
			.enabledExtensionCount = static_cast<uint32_t>(_extensions.size()),
			.ppEnabledExtensionNames = _extensions.data(),
		};
		
		_logicalDevice = _physicalDevice.createDevice(createInfo);
		VULKAN_HPP_DEFAULT_DISPATCHER.init(_logicalDevice);

		_grapichsQueue = _logicalDevice.getQueue(indices.graphicsFamily.value(), 0);
		_presentQueue = _logicalDevice.getQueue(indices.presentFamily.value(), 0);
	}

	void RenderContext::CreateContextCommandPool() {
		QueueFamilyIndices indices = FindPhysicalDeviceQueueFamilies();
		vk::CommandPoolCreateInfo createInfo{
			.flags = vk::CommandPoolCreateFlagBits::eProtected | vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			.queueFamilyIndex = indices.graphicsFamily.value()
		};
		_commandPool = _logicalDevice.createCommandPool(createInfo);
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
		device.enumerateDeviceExtensionProperties(nullptr, &exensionCount, nullptr);
		std::vector<vk::ExtensionProperties> availableExtensions(exensionCount);
		device.enumerateDeviceExtensionProperties(nullptr, &exensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(_extensions.begin(), _extensions.end());
		for (const auto& extension : availableExtensions)
			requiredExtensions.erase(extension.extensionName);
		return requiredExtensions.empty();
	}

	bool RenderContext::CheckValidationLayerSupport()
	{
		uint32_t layerCount;
		vk::enumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<vk::LayerProperties> availableLayers(layerCount);
		vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data());

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
		device.getQueueFamilyProperties(&queueFamilyCount, nullptr);
		std::vector<vk::QueueFamilyProperties> queueFamilies(queueFamilyCount);
		device.getQueueFamilyProperties(&queueFamilyCount, queueFamilies.data());
			
		for (int i = 0; i < queueFamilyCount; i++) {
			const auto& queueFamily = queueFamilies[i];
			if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
				indices.graphicsFamily = i;

			bool presentSupport = device.getSurfaceSupportKHR(i, _surface);
			if (presentSupport)
				indices.presentFamily = i;

			if (indices.IsComplete())
				break;
		}
		return indices;
	}

	uint32_t RenderContext::FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		_physicalDevice.getMemoryProperties(&memProperties);
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}
		CHECKF(false, "Failed to find any usable memory type");
	}

	vk::CommandBuffer RenderContext::BeginSingleTimeCommand() {
		vk::CommandBufferAllocateInfo allocInfo{
			.commandPool = _commandPool,
			.level = vk::CommandBufferLevel::ePrimary,
			.commandBufferCount = 1
		};
		vk::CommandBuffer cmd;
		_logicalDevice.allocateCommandBuffers(allocInfo, &cmd);

		vk::CommandBufferBeginInfo beginInfo{
			.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit
		};
		cmd.begin(beginInfo);
		return cmd;
	}

	void RenderContext::EndSingleTimeCommand(vk::CommandBuffer cmd) {
		cmd.end();
		vk::SubmitInfo submitInfo{
			.commandBufferCount = 1,
			.pCommandBuffers = &cmd
		};
		_grapichsQueue.submit(submitInfo);
		_grapichsQueue.waitIdle();
		_logicalDevice.freeCommandBuffers(_commandPool, cmd);
	}
}