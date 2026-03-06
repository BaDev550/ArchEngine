#pragma once

#include <imgui.h>
#include <vulkan/vulkan.hpp>

namespace ae::grapichs {
	class ImGuiRenderer {
	public:
		static void Init();
		static void Destroy();
		static void Begin();
		static void End();
	};
}