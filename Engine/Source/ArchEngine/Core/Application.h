#pragma once
#include "Window.h"
#include "Memory.h"

namespace ae {
	using namespace memory;
	struct ApplicationSpecifications {
		uint32_t Width;
		uint32_t Height;
		std::string Title;
	};
	class Application {
	public:
		Application();

		void Run();
		static Application* Get() { return _instance; }

		virtual void ApplicationUpdate() = 0;
	private:
		Scope<Window> _window = nullptr;

		static Application* _instance;
	};
}
ae::Application* CreateApplication();