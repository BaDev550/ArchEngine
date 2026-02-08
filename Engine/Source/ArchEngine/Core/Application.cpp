#include "ArchPch.h"
#include "Application.h"
#include "ArchEngine/Utilities/Profiler.h"
#include <iostream>

namespace ae {
	Application* Application::_instance = nullptr;
	Application::Application()
	{
		if (_instance)
			return;
		_instance = this;
		Logger_app::info("Application Created!");

		PROFILE_SCOPE("WindowCreate");
		_window = MakeScope<Window>(WindowSpecifications());
	}

	void Application::Run()
	{
		while (!_window->ShoudClose()) {
			_window->PoolEvents();

			ApplicationUpdate();
		}
		_window->GetRenderContext().WaitDeviceIdle();
		profiler::utils::WriteProfileToFile("ArchEngineApplicationProfile.json");
	}
}