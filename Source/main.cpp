#include <iostream>
#include <memory>
#include "Core/Window.h"

using namespace ae;
int main() {
	{
		memory::Scope<Window> window = memory::MakeScope<Window>(WindowSpecifications(800, 800, "Server"));

		while (!window->ShoudClose()) {
			window->PoolEvents();
		}
	}

	_CrtDumpMemoryLeaks();
	return 0;
}