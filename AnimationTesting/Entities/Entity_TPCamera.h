#pragma once
#include <ArchEngine/Objects/Entity_Camera.h>
#include <ArchEngine/Core/Input.h>
#include <ArchEngine/Scene/Scene.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Entity_TPCamera : public ae::Entity_Camera {
public:
    REGISTER_ENTITY(Entity_TPCamera);

    virtual void OnCreate() override {
        ae::Entity_Camera::OnCreate();
        SetName("TPCamera");
        _lastMousePos = ae::Input::GetMousePosition();
    }

    virtual void OnUpdate(float deltaTime) override {
        if (_targetEntityID == 0) return;

        ae::Entity_Camera::OnUpdate(deltaTime);
        ae::Entity* target = GetScene()->GetEntity(_targetEntityID);
        if (!target) return;

        glm::vec2 currentMousePos = -ae::Input::GetMousePosition();
        glm::vec2 delta = currentMousePos - _lastMousePos;
        _lastMousePos = currentMousePos;

        _boomYaw -= delta.x * _mouseSensitivity * deltaTime;
        _boomPitch += delta.y * _mouseSensitivity * deltaTime;
        _boomPitch = glm::clamp(_boomPitch, -85.0f, 85.0f);

        glm::vec3 rawTargetPos = target->GetPosition() + _targetOffset;

        if (_enableCameraLag) {
            _currentBoomRoot = glm::mix(_currentBoomRoot, rawTargetPos, _cameraLagSpeed * deltaTime);
        }
        else {
            _currentBoomRoot = rawTargetPos;
        }

        glm::vec3 boomDirection;
        boomDirection.x = cos(glm::radians(_boomYaw)) * cos(glm::radians(_boomPitch));
        boomDirection.y = sin(glm::radians(_boomPitch));
        boomDirection.z = sin(glm::radians(_boomYaw)) * cos(glm::radians(_boomPitch));
        boomDirection = glm::normalize(boomDirection);

        float currentArmLength = _targetArmLength;
        glm::vec3 desiredCameraPos = _currentBoomRoot - (boomDirection * currentArmLength);

        GetCamera()->SetPosition(desiredCameraPos);

        glm::vec3 lookDir = glm::normalize(_currentBoomRoot - desiredCameraPos);
        float targetYaw = glm::degrees(atan2(lookDir.x, lookDir.z));
        float targetPitch = glm::degrees(asin(lookDir.y));

        GetCamera()->SetRotation({ -targetPitch, targetYaw, 0.0f });
    }

    void SetTarget(ae::EntityID targetID) { _targetEntityID = targetID; }

    float _targetArmLength = 2.5f;
    glm::vec3 _targetOffset = glm::vec3(0.0f, 2.35f, 0.0f);

    bool _enableCameraLag = true;
    float _cameraLagSpeed = 5.0f;

    virtual void OnSerialize(YAML::Emitter& out) override {
        out << YAML::Key << "TargetEntity" << YAML::Value << (uint64_t)_targetEntityID;
    };

    virtual void OnDeserialize(YAML::Node& data) override {
        uint64_t targetEntity = data["TargetEntity"].as<uint64_t>();
        _targetEntityID = targetEntity;
    };

private:
    ae::EntityID _targetEntityID = 0;

    float _boomPitch = 20.0f;
    float _boomYaw = 0.0f;
    float _mouseSensitivity = 1.0f;
    glm::vec2 _lastMousePos = { 0.0f, 0.0f };

    glm::vec3 _currentBoomRoot = glm::vec3(0.0f);
};