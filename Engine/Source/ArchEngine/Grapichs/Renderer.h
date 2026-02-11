#pragma once
#include "ShaderLibrary.h"
#include <iostream>

namespace ae::grapichs {
	class Renderer {
	public:
		static inline uint32_t MaxFramesInFlight = 2;

		static void Init();
		static void Destroy();

		static vk::CommandBuffer GetCurrentCommandBuffer();
		static uint32_t GetFrameIndex();
		static ShaderLibrary& GetShaderLibrary();
	};
}