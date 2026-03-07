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
		_window->SetCursor(false);

		PhysicsEngine::Init(PhysicsSettings());
		AssetManager::Init();
		Input::Init();
		Renderer::Init();
		ImGuiRenderer::Init();
	}

	Application::~Application() {
		profiler::Profiler::Get().Clear();
		profiler::Profiler::Destroy();
		PhysicsEngine::Shutdown();
		AssetManager::Destroy();
		ImGuiRenderer::Destroy();
		Renderer::Destroy();
		_window = nullptr;
	}

	void Application::Run()
	{
		ApplicationStarted();
		while (!_window->ShoudClose()) {
			static auto lastFrameTime = std::chrono::high_resolution_clock::now();
			auto currentTime = std::chrono::high_resolution_clock::now();
			_deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
			lastFrameTime = currentTime;

			_window->PoolEvents();

			Input::Update();
			ApplicationUpdate();
		}
		_window->GetRenderContext().WaitDeviceIdle();
		profiler::utils::WriteProfileToFile("ArchEngineApplicationProfile.json");
	}
}