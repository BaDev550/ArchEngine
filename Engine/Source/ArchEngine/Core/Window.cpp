#include "ArchPch.h"
#include "Window.h"
#include <assert.h>

namespace ae {
	static bool s_GLFWInitialized = false;
	Window::Window(WindowSpecifications windowSpecs)
		: _specs(windowSpecs)
	{
		{
			PROFILE_SCOPE("WindowCreate");
			if (!s_GLFWInitialized) {
				bool success = glfwInit();
				assert(success);
				glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
				glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
			}
			_handle = glfwCreateWindow(_specs.Width, _specs.Height, _specs.Title.c_str(), nullptr, nullptr);
			glfwMakeContextCurrent(_handle);
		}

		{
			_renderContext = new grapichs::RenderContext(this);
		}
		{
			_swapchain = new grapichs::Swapchain(*_renderContext);
		}
	}

	Window::~Window()
	{
		delete _swapchain;
		_swapchain = nullptr;

		delete _renderContext;
		_renderContext = nullptr;

		glfwDestroyWindow(_handle);
		glfwTerminate();
	}

	GLFWwindow* Window::GetHandle() const
	{
		return _handle;
	}

	void Window::SwapBuffers()
	{
		_swapchain->Swapbuffers(&_imageIndex);
	}

	void Window::PoolEvents() const
	{
		glfwPollEvents();
	}

	void Window::SetCursor(bool enabled) {
		glfwSetInputMode(_handle, GLFW_CURSOR, enabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
	}

	bool Window::ShoudClose() const
	{
		return glfwWindowShouldClose(_handle);
	}

	void Window::CreateDefaultSwapchainFramebuffer() {
		grapichs::FramebufferSpecification specs{};
		specs.IsSwapchain = true;
		specs.DepthClearValue = 1.0f;
		specs.ClearColor = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
		_defaultFramebuffer = memory::Ref<grapichs::Framebuffer>::Create(specs);
	}
}