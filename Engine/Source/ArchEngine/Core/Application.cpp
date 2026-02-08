#include "Application.h"
#include <iostream>

namespace ae {
	Application* Application::_instance = nullptr;
	Application::Application()
	{
		if (_instance)
			return;
		_instance = this;
		std::cout << "Application Created! \n";

		_window = MakeScope<Window>(WindowSpecifications());
	}

	void Application::Run()
	{
		while (!_window->ShoudClose()) {
			_window->PoolEvents();

			ApplicationUpdate();
		}
	}
}