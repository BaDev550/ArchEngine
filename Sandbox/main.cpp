#include <iostream>
#include <memory>
#include <ArchEngine/Core/Application.h>
#include <ArchEngine/Core/EntryPoint.h>

using namespace ae;
class SandboxGame : public Application {
public:
	SandboxGame() {}
	~SandboxGame() = default;

	virtual void ApplicationUpdate() override {

	}
};

ae::Application* CreateApplication() {
	return new SandboxGame();
}