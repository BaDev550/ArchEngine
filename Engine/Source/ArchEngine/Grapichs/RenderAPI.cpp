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
            frame.CommandBuffer = _context.GetDevice().allocateCommandBuffers(allocInfo).at(0);
        }
    }

    RenderAPI::~RenderAPI()
    {
        for (uint32_t i = 0; i < Renderer::MaxFramesInFlight; i++) {
            _context.GetDevice().freeCommandBuffers(_frames[i].CommandPool, _frames[i].CommandBuffer);
            _context.GetDevice().destroyCommandPool(_frames[i].CommandPool);
        }
    }

    void RenderAPI::BeginFrame()
    {
        CHECKF(!_frameStarted, "Cannot call BeginFrame while processing a frame");
        _frameStarted = true;
        uint32_t frameIndex = Renderer::GetFrameIndex();
        FrameContext& frame = _frames[frameIndex];
        vk::CommandBufferBeginInfo beginInfo{};

        Application::Get()->GetWindow().SwapBuffers();

        _context.GetDevice().resetCommandPool(frame.CommandPool);
        frame.CommandBuffer.begin(beginInfo);
    }

    void RenderAPI::EndFrame()
    {
        CHECKF(_frameStarted, "Cannot call EndFrame while not processing a frame");
        vk::CommandBuffer cmd = GetCurrentCommandBuffer();
        uint32_t frameIndex = Renderer::GetFrameIndex();
        auto& window = Application::Get()->GetWindow();
        auto& swapchain = window.GetSwapchain();
        uint32_t imageIndex = window.GetImageIndex();

        cmd.end();
        vk::Result result = swapchain.Submit(&cmd, &imageIndex);
        if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || window.Resized()) {
            window.ResetResizedFlag();
            window.RecreateDefaultSwapchainAndFramebuffer();
        }
        _renderStats.DrawCalls = 0;
        _frameStarted = false;
    }

    void RenderAPI::DrawVertex(vk::CommandBuffer cmd, memory::Ref<Buffer> vertexBuffer, uint32_t vertexCount)
    {
        if (vertexBuffer) {
            vk::Buffer vertexBuffers[] = { vertexBuffer->GetBuffer() };
            vk::DeviceSize offsets[] = { 0 };
            cmd.bindVertexBuffers(0, vertexBuffers, offsets);
        }
        cmd.draw(vertexCount, 1, 0, 0);
        _renderStats.DrawCalls++;
    }

    void RenderAPI::DrawIndexed(vk::CommandBuffer cmd, memory::Ref<Buffer>& vertexBuffer, memory::Ref<Buffer>& indexBuffer, uint32_t indexCount)
    {
		vk::Buffer vertexBuffers[] = { vertexBuffer->GetBuffer() };
		vk::DeviceSize offsets[] = { 0 };
		cmd.bindVertexBuffers(0, vertexBuffers, offsets);
		cmd.bindIndexBuffer(indexBuffer->GetBuffer(), 0, vk::IndexType::eUint32);
        cmd.drawIndexed(indexCount, 1, 0, 0, 0);
        _renderStats.DrawCalls++;
    }

    void RenderAPI::DrawStaticMesh(memory::Ref<RenderPass>& renderPass, vk::CommandBuffer cmd, memory::Ref<MeshSource>& meshSource, memory::Ref<StaticMesh>& staticMesh)
    {
        vk::Buffer vertexBuffers[] = { meshSource->GetVertexBuffer()->GetBuffer()};
        vk::DeviceSize offsets[] = { 0 };
        cmd.bindVertexBuffers(0, vertexBuffers, offsets);
        cmd.bindIndexBuffer(meshSource->GetIndexBuffer()->GetBuffer(), 0, vk::IndexType::eUint32);

        for (const auto& submesh : meshSource->GetSubmeshes()) {
            memory::Ref<MaterialAsset>& materialAsset = staticMesh->GetMaterialByID(submesh.MaterialIndex);
            memory::Ref<Material>& material = materialAsset->GetMaterial();

            material->Bind(cmd, renderPass->GetPipeline()->GetPipelineLayout());
            cmd.drawIndexed(submesh.IndexCount, 1, submesh.IndexOffset, submesh.VertexOffset, 0);
            _renderStats.DrawCalls++;
        }
    }

    void RenderAPI::DrawStaticMeshEntity(memory::Ref<RenderPass>& renderPass, vk::CommandBuffer cmd, memory::Ref<MeshSource>& meshSource, memory::Ref<StaticMesh>& staticMesh, const glm::mat4& transform)
    {
        vk::Buffer vertexBuffers[] = { meshSource->GetVertexBuffer()->GetBuffer() };
        vk::DeviceSize offsets[] = { 0 };
        cmd.bindVertexBuffers(0, vertexBuffers, offsets);
        cmd.bindIndexBuffer(meshSource->GetIndexBuffer()->GetBuffer(), 0, vk::IndexType::eUint32);
        cmd.pushConstants(renderPass->GetPipeline()->GetPipelineLayout(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4), &transform);

        for (const auto& submesh : meshSource->GetSubmeshes()) {
            cmd.drawIndexed(submesh.IndexCount, 1, submesh.IndexOffset, submesh.VertexOffset, 0);
            _renderStats.DrawCalls++;
        }
    }

    void RenderAPI::DrawStaticMeshEntityWithMaterial(memory::Ref<RenderPass>& renderPass, vk::CommandBuffer cmd, memory::Ref<MeshSource>& meshSource, memory::Ref<StaticMesh>& staticMesh, const glm::mat4& transform) {
        vk::Buffer vertexBuffers[] = { meshSource->GetVertexBuffer()->GetBuffer() };
        vk::DeviceSize offsets[] = { 0 };
        cmd.bindVertexBuffers(0, vertexBuffers, offsets);
        cmd.bindIndexBuffer(meshSource->GetIndexBuffer()->GetBuffer(), 0, vk::IndexType::eUint32);
        cmd.pushConstants(renderPass->GetPipeline()->GetPipelineLayout(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4), &transform);
        
        for (const auto& submesh : meshSource->GetSubmeshes()) {
            memory::Ref<MaterialAsset>& materialAsset = staticMesh->GetMaterialByID(submesh.MaterialIndex);
            memory::Ref<Material>& material = materialAsset->GetMaterial();

            material->Bind(cmd, renderPass->GetPipeline()->GetPipelineLayout());
            cmd.drawIndexed(submesh.IndexCount, 1, submesh.IndexOffset, submesh.VertexOffset, 0);
            _renderStats.DrawCalls++;
        }
    }

    vk::CommandBuffer RenderAPI::GetCurrentCommandBuffer()
    {
        CHECKF(_frameStarted, "Cannot get active command buffer while frame is not started");
        return _frames[Renderer::GetFrameIndex()].CommandBuffer;
    }
}