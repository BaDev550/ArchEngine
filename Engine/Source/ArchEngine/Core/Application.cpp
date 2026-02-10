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
		profiler::Profiler::Init();
		Logger_app::info("Application Created!");
		
		_window = MakeScope<Window>(WindowSpecifications());

		Renderer::Init();
	}

	Application::~Application() {
		Renderer::Destroy();
		profiler::Profiler::Get().Clear();
		profiler::Profiler::Destroy();
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