#pragma once
#include <glfw/glfw3.h>
#include <string>

#include "ArchEngine/Grapichs/RenderContext.h"
#include "ArchEngine/Grapichs/Swapchain.h"

namespace ae {
	struct WindowSpecifications {
		uint32_t Width;
		uint32_t Height;
		std::string Title;

		WindowSpecifications(uint32_t width = 800, uint32_t height = 800, const std::string& title = "Game Window")
			: Width(width), Height(height), Title(title) {}
	};

	class Window {
	public:
		Window(WindowSpecifications windowSpecs);
		~Window();

		uint32_t GetWidth() const { return _specs.Width; }
		uint32_t GetHeight() const { return _specs.Height; }
		grapichs::RenderContext& GetRenderContext() { return *_renderContext; }
		grapichs::Swapchain& GetSwapchain() { return *_swapchain; }
		GLFWwindow* GetHandle() const;
		void PoolEvents() const;
		bool ShoudClose() const;
	private:
		GLFWwindow* _handle;
		grapichs::RenderContext* _renderContext;
		grapichs::Swapchain* _swapchain;
		WindowSpecifications _specs;
	};
}