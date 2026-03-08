#pragma once
#include "ArchEngine/Core/Memory.h"
#include "ArchEngine/Grapichs/Buffer.h"
#include "ArchEngine/Grapichs/Pipeline.h"
#include "ArchEngine/Grapichs/DescriptorManager.h"
#include "ArchEngine/Grapichs/Camera.h"
#include "ArchEngine/Grapichs/TextureCube.h"

namespace ae::grapichs {
	class Skybox : public memory::RefCounted {
	public:
		Skybox(const memory::Ref<Framebuffer>& targetFramebuffer,
			const memory::Ref<Buffer>& cameraBuffer,
			const memory::Ref<TextureCube>& environmentMap);
		~Skybox();
		void DrawSkybox();
		void SetEnviromentMap(memory::Ref<TextureCube>& environmentMap);
		const memory::Ref<TextureCube>& GetEnvironmentMap() const { return _environmentMap; }
	private:
		memory::Ref<Buffer> _skyboxVertexBuffer;
		memory::Ref<Pipeline> _skyboxPipeline;
		memory::Ref<DescriptorManager> _descriptorManager;
		memory::Ref<TextureCube> _environmentMap;
	};
}