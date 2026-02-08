#pragma once
#include <glfw/glfw3.h>
#include "Memory.h"
#include <string>

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

		GLFWwindow* GetHandle() const;
		void PoolEvents() const;
		bool ShoudClose() const;
	private:
		GLFWwindow* _handle;
		WindowSpecifications _specs;
	};
}