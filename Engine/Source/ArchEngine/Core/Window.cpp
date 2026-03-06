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
				glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
			}
			_handle = glfwCreateWindow(_specs.Width, _specs.Height, _specs.Title.c_str(), nullptr, nullptr);
			glfwMakeContextCurrent(_handle);
			glfwSetWindowUserPointer(_handle, this);
			glfwSetFramebufferSizeCallback(_handle, FramebufferResizeCallback);
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

	void Window::SetClearColor(glm::vec4 clearColor) {
		_defaultFramebuffer->GetSpecification().ClearColor = clearColor;
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
		_defaultFramebufferSpecs.IsSwapchain = true;
		_defaultFramebufferSpecs.DepthClearValue = 1.0f;
		_defaultFramebufferSpecs.ClearColor = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
		_defaultFramebuffer = memory::Ref<grapichs::Framebuffer>::Create(_defaultFramebufferSpecs);
	}

	void Window::RecreateDefaultSwapchainAndFramebuffer() {
		_defaultFramebufferSpecs.Width = _specs.Width;
		_defaultFramebufferSpecs.Height = _specs.Height;
		_swapchain->Recreate();
		_defaultFramebuffer->Invalidate(_defaultFramebufferSpecs);
	}

	void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height) {
		Window* win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		win->_specs.Width = width;
		win->_specs.Height = height;
		win->_resized = true;
	}
}