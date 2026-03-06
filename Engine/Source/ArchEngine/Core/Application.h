#pragma once
#include "Window.h"
#include "Memory.h"
#include "ArchEngine/Grapichs/Renderer.h"
#include "ArchEngine/Grapichs/ImGuiRenderer.h"
#include "ArchEngine/Utilities/Logger.h"

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
		virtual void ApplicationStarted() = 0;
		virtual void ApplicationUpdate() = 0;

		static Application* Get() { return _instance; }
		Window& GetWindow() { return *_window; }
	protected:
		float _deltaTime;
		Scope<Window> _window = nullptr;
	private:
		static Application* _instance;
	};
}
ae::Application* CreateApplication();