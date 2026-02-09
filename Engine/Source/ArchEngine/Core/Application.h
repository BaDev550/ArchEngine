#pragma once
#include "Window.h"
#include "Memory.h"
#include "ArchEngine/Grapichs/Renderer.h"
#include "ArchEngine/Utilities/Logger.h"

#include "ArchEngine/Grapichs/Shader.h"
#include "ArchEngine/Grapichs/Pipeline.h"

namespace ae {
	using namespace memory;
	using namespace grapichs;
	struct ApplicationSpecifications {
		uint32_t Width;
		uint32_t Height;
		std::string Title;
	};

	class Application {
	public:
		Application();
		virtual ~Application();

		void Run();
		virtual void ApplicationUpdate() = 0;

		static Application* Get() { return _instance; }
		Window& GetWindow() { return *_window; }
	private:
		Scope<Window> _window = nullptr;

		Ref<Shader> _shader = nullptr;
		Ref<Pipeline> _pipeline = nullptr;

		static Application* _instance;
	};
}
ae::Application* CreateApplication();