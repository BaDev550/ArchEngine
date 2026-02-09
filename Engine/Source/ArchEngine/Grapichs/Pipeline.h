#pragma once
#include "ArchEngine/Core/Memory.h"

#include "RenderContext.h"
#include "Shader.h"

namespace ae::grapichs {
	struct PipelineData {
		memory::Ref<Shader> Shader;
	};

	class Pipeline : public memory::RefCounted {
	public:
		Pipeline(const PipelineData& data);
		~Pipeline();
		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;

		PipelineData& GetPipelineData() { return _data; }
		vk::Pipeline GetPipeline() const { return _pipeline; }
		vk::PipelineLayout GetPipelineLayout() const { return _pipelineLayout; }

		void Invalidate();
		const memory::Ref<Shader>& GetShader() const { return _data.Shader; }
	private:
		vk::Pipeline _pipeline;
		vk::PipelineCache _pipelineCache;
		vk::PipelineLayout _pipelineLayout;
		PipelineData _data;

		RenderContext& _context;
	};
}