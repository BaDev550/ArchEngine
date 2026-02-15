#include "ArchPch.h"
#include "RenderAPI.h"
#include "ArchEngine/Core/Application.h"

namespace ae::grapichs {
    RenderAPI::RenderAPI()
        : _context(Application::Get()->GetWindow().GetRenderContext())
    {
        for (uint32_t i = 0; i < Renderer::MaxFramesInFlight; i++) {
            auto& frame = _frames[i];

            vk::CommandPoolCreateInfo poolInfo{};
            poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
            poolInfo.queueFamilyIndex = _context.FindPhysicalDeviceQueueFamilies().graphicsFamily.value();
            frame.CommandPool = _context.GetDevice().createCommandPool(poolInfo);

            vk::CommandBufferAllocateInfo allocInfo{};
            allocInfo.commandBufferCount = 1;
            allocInfo.level = vk::CommandBufferLevel::ePrimary;
            allocInfo.commandPool = frame.CommandPool;
            auto cmds = _context.GetDevice().allocateCommandBuffers(allocInfo);
            frame.CommandBuffer = cmds[0];
        }
    }

    RenderAPI::~RenderAPI()
    {
    }

    void RenderAPI::BeginFrame()
    {
    }

    void RenderAPI::EndFrame()
    {
    }

    vk::CommandBuffer RenderAPI::GetCurrentCommandBuffer()
    {
        return vk::CommandBuffer();
    }
}