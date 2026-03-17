#pragma once
#include <ArchEngine/Objects/Entity.h>
#include <ArchEngine/Grapichs/Camera.h>
#include <ArchEngine/Core/Memory.h>

namespace ae {
    class Entity_Camera : public ae::Entity {
    public:
        REGISTER_ENTITY(Entity_Camera);

        virtual void OnCreate() override {
            _camera = memory::Ref<grapichs::Camera>::Create();
            SetName("Camera");
        }

        virtual void OnUpdate(float deltaTime) override {
            _camera->CalculateCameraMatrixes();
        }

        virtual void OnDestroy() override {}

        memory::Ref<grapichs::Camera> GetCamera() const { return _camera; }
    private:
        memory::Ref<grapichs::Camera> _camera = nullptr;
    };
}