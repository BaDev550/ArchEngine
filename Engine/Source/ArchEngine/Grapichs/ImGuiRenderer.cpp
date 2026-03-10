#include "ArchPch.h"
#include "ImGuiRenderer.h"

#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>
#include <ImGuizmo.h>

#include "RenderContext.h"
#include "ArchEngine/Core/Application.h"

namespace ae::grapichs {
	static vk::DescriptorPool s_descriptorPool;
	
	void ImGuiRenderer::Init() {
		auto& window = Application::Get()->GetWindow();
		auto& context = window.GetRenderContext();

		std::vector<vk::DescriptorPoolSize> poolSize;
		poolSize.push_back({ vk::DescriptorType::eUniformBuffer,        1000 });
		poolSize.push_back({ vk::DescriptorType::eStorageImage,         1000 });
		poolSize.push_back({ vk::DescriptorType::eCombinedImageSampler, 1000 });
		poolSize.push_back({ vk::DescriptorType::eSampledImage,         1000 });
		poolSize.push_back({ vk::DescriptorType::eUniformTexelBuffer,   1000 });
		poolSize.push_back({ vk::DescriptorType::eStorageTexelBuffer,   1000 });
		poolSize.push_back({ vk::DescriptorType::eUniformBufferDynamic, 1000 });
		poolSize.push_back({ vk::DescriptorType::eStorageBufferDynamic, 1000 });
		poolSize.push_back({ vk::DescriptorType::eInputAttachment,      1000 });

		vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo{};
		descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSize.size());
		descriptorPoolCreateInfo.pPoolSizes = poolSize.data();
		descriptorPoolCreateInfo.maxSets = 1000;
		descriptorPoolCreateInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
		s_descriptorPool = context.GetDevice().createDescriptorPool(descriptorPoolCreateInfo);

		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		ImGui_ImplGlfw_InitForVulkan(window.GetHandle(), true);
		ImGui_ImplVulkan_PipelineInfo pipelineInfo{};
		VkFormat swapchainColorImageFormat = (VkFormat)window.GetSwapchain().GetSwapchainFormat();
		VkFormat swapchainDepthImageFormat = (VkFormat)window.GetSwapchain().GetSwapchainDepthFormat();
		pipelineInfo.RenderPass = nullptr;
		pipelineInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		pipelineInfo.PipelineRenderingCreateInfo.sType = { VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
		pipelineInfo.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
		pipelineInfo.PipelineRenderingCreateInfo.pColorAttachmentFormats = &swapchainColorImageFormat;
		pipelineInfo.PipelineRenderingCreateInfo.depthAttachmentFormat = swapchainDepthImageFormat;

		ImGui_ImplVulkan_InitInfo initInfo{};
		initInfo.Instance = context.GetInstance();
		initInfo.PhysicalDevice = context.GetPhysicalDevice();
		initInfo.Device = context.GetDevice();
		initInfo.Queue = context.GetGrapichsQueue();
		initInfo.DescriptorPool = s_descriptorPool;
		initInfo.MinImageCount = 3;
		initInfo.ImageCount = 3;
		initInfo.UseDynamicRendering = true;
		initInfo.PipelineInfoMain = pipelineInfo;
		ImGui_ImplVulkan_Init(&initInfo);
		ImGui_ImplVulkan_CreateMainPipeline(&pipelineInfo);
		ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());
	}

	void ImGuiRenderer::Destroy(){
		auto& window = Application::Get()->GetWindow();
		auto& context = window.GetRenderContext();
		ImGui_ImplGlfw_Shutdown();
		ImGui_ImplVulkan_Shutdown();
		context.GetDevice().destroyDescriptorPool(s_descriptorPool);
		ImGui::DestroyContext();
	}

	void ImGuiRenderer::Begin(){
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiRenderer::End() {
		ImGui::Render();
		
		auto cmd = Renderer::GetCurrentCommandBuffer();
		auto& window = Application::Get()->GetWindow();
		auto& context = window.GetRenderContext();
		auto& swapchain = window.GetSwapchain();

		VkExtent2D extent = swapchain.GetSwapChainExtent();

		VkRenderingAttachmentInfo colorAttachment{};
		colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		colorAttachment.imageView = swapchain.GetSwapChainImageView(Application::Get()->GetWindow().GetImageIndex());
		colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		VkRenderingInfo renderingInfo{};
		renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		renderingInfo.renderArea = { {0, 0}, extent };
		renderingInfo.layerCount = 1;
		renderingInfo.colorAttachmentCount = 1;
		renderingInfo.pColorAttachments = &colorAttachment;

		vkCmdBeginRendering(cmd, &renderingInfo);
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
		vkCmdEndRendering(cmd);
	}
}