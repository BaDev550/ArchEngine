#pragma once

#include <iostream>

namespace AE {
	class Engine {
	public:
		static void Init() { std::cout << "Engine function called" << std::endl; }
	};
}