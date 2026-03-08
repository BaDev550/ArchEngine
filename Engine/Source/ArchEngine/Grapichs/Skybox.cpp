#include "ArchPch.h"
#include "Skybox.h"

#include "ArchEngine/Grapichs/Renderer.h"

namespace ae::grapichs {
    Skybox::Skybox(const memory::Ref<Framebuffer>& targetFramebuffer,
        const memory::Ref<Buffer>& cameraBuffer,
        const memory::Ref<TextureCube>& environmentMap)
        : _environmentMap(environmentMap) {
        PipelineData skyboxData{};
        skyboxData.TargetFramebuffer = targetFramebuffer;
        skyboxData.Shader = Renderer::GetShaderLibrary().GetShader("SkyboxShader");
        skyboxData.RenderData.DepthTestEnable = false;
        skyboxData.RenderData.CullingEnable = false;

        _skyboxPipeline = memory::Ref<Pipeline>::Create(skyboxData);

        _descriptorManager = memory::Ref<DescriptorManager>::Create(skyboxData.Shader);
        _descriptorManager->Invalidate();
        _descriptorManager->WriteInput("uCamera", cameraBuffer);

        float skyboxVertices[] = {   
            -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,   1.0f,  1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,  -1.0f, -1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,   1.0f, -1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,   1.0f,  1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,   1.0f, -1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,   1.0f,  1.0f, -1.0f,   1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,   1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,   1.0f, -1.0f,  1.0f
        };

        _skyboxVertexBuffer = memory::Ref<Buffer>::Create(
            sizeof(skyboxVertices),
            vk::BufferUsageFlagBits::eVertexBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
        );
        _skyboxVertexBuffer->Map();
        _skyboxVertexBuffer->Write(skyboxVertices, sizeof(skyboxVertices));
        _skyboxVertexBuffer->Unmap();
	}

	Skybox::~Skybox() {

	}

    void Skybox::SetEnviromentMap(memory::Ref<TextureCube>& environmentMap) {
        _environmentMap = environmentMap;
        _descriptorManager->WriteInput("uSkyboxTexture", _environmentMap);
        _descriptorManager->Bake();
    }

    void Skybox::DrawSkybox() {
        if (!_environmentMap) return;

        vk::CommandBuffer cmd = grapichs::Renderer::GetCurrentCommandBuffer();


        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, _skyboxPipeline->GetPipeline());
        _descriptorManager->BindSets(cmd, _skyboxPipeline->GetPipelineLayout());
        grapichs::Renderer::DrawVertex(cmd, _skyboxVertexBuffer, 36);
	}
}