#pragma once
#include "Window.h"
#include "Memory.h"
#include "ArchEngine/Grapichs/Renderer.h"
#include "ArchEngine/Grapichs/ImGuiRenderer.h"
#include "ArchEngine/Physics/PhysicsEngine.h"
#include "ArchEngine/AssetManager/AssetManager.h"
#include "ArchEngine/Utilities/Logger.h"
#include <ArchEngine/Core/Input.h>

namespace ae {
	using namespace memory;
	using namespace grapichs;
	using namespace physics;
	struct ApplicationSpecifications {
		uint32_t Width;
		uint32_t Height;
		std::string Title;
	};

	class Application {
	public:
		Application(const ApplicationSpecifications& specs);
		virtual ~Application();

		void Run();
		virtual void ApplicationStarted() = 0;
		virtual void ApplicationUpdate() = 0;

		static Application* Get() { return _instance; }
		Window& GetWindow() { return *_window; }
		float GetDeltaTime() const { return _deltaTime; }
	protected:
		float _deltaTime;
		ApplicationSpecifications _specs;
		Scope<Window> _window = nullptr;
	private:
		static Application* _instance;
	};
}
ae::Application* CreateApplication();