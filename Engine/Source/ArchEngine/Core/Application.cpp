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
		_window->CreateDefaultSwapchainFramebuffer();

		Renderer::Init();
	}

	Application::~Application() {
		profiler::Profiler::Get().Clear();
		profiler::Profiler::Destroy();
		Renderer::Destroy();
	}

	void Application::Run()
	{
		ApplicationStarted();
		while (!_window->ShoudClose()) {
			_window->PoolEvents();

			ApplicationUpdate();
		}
		_window->GetRenderContext().WaitDeviceIdle();
		profiler::utils::WriteProfileToFile("ArchEngineApplicationProfile.json");
	}
}