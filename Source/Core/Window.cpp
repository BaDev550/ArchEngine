#include "Window.h"
#include <assert.h>

namespace ae {
	static bool s_GLFWInitialized = false;
	Window::Window(WindowSpecifications windowSpecs)
		: _specs(windowSpecs)
	{
		if (!s_GLFWInitialized) {
			bool success = glfwInit();
			assert(success);
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		}
		_handle = glfwCreateWindow(_specs.Width, _specs.Height, _specs.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(_handle);
	}

	Window::~Window()
	{
		glfwDestroyWindow(_handle);
		glfwTerminate();
	}

	GLFWwindow* Window::GetHandle() const
	{
		return _handle;
	}

	void Window::PoolEvents() const
	{
		glfwPollEvents();
	}

	bool Window::ShoudClose() const
	{
		return glfwWindowShouldClose(_handle);
	}
}