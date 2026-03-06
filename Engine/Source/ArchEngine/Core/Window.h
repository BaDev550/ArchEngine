#pragma once
#include <glfw/glfw3.h>
#include <string>

#include "ArchEngine/Grapichs/RenderContext.h"
#include "ArchEngine/Grapichs/Swapchain.h"
#include "ArchEngine/Grapichs/Framebuffer.h"

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

		void SwapBuffers();
		void SetClearColor(glm::vec4 clearColor);
		uint32_t GetImageIndex() const { return _imageIndex; }
		uint32_t GetWidth() const { return _specs.Width; }
		uint32_t GetHeight() const { return _specs.Height; }
		grapichs::Framebuffer* GetDefaultSwapchainFramebuffer() { return _defaultFramebuffer.Get(); }
		grapichs::RenderContext& GetRenderContext() { return *_renderContext; }
		grapichs::Swapchain& GetSwapchain() { return *_swapchain; }
		GLFWwindow* GetHandle() const;
		void PoolEvents() const;
		void SetCursor(bool enabled);
		bool ShoudClose() const;
		void RecreateDefaultSwapchainAndFramebuffer();
		void CreateDefaultSwapchainFramebuffer();
		bool Resized() const { return _resized; }
		void ResetResizedFlag() { _resized = false; }
	private:
		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

		GLFWwindow* _handle;
		uint32_t _imageIndex;
		grapichs::RenderContext* _renderContext;
		grapichs::Swapchain* _swapchain;
		grapichs::FramebufferSpecification _defaultFramebufferSpecs;
		memory::Ref<grapichs::Framebuffer> _defaultFramebuffer = nullptr;
		WindowSpecifications _specs;
		bool _resized = false;
	};
}