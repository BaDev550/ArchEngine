#pragma once
#include "ShaderLibrary.h"

namespace ae::grapichs {
	class Renderer {
	public:
		static void Init();
		static void Destroy();

		static ShaderLibrary& GetShaderLibrary();
	};
}