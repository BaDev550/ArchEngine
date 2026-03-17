#pragma once
#include <iostream>
#include "ArchEngine/Core/Memory.h"

//#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ae::grapichs {
	class Camera : public memory::RefCounted {
	public:
		~Camera() = default;
		virtual void Update(float dt) {}

		void Orbit(float pitchOffset, float yawOffset);
		void SetFirstMouse() { _FirstMouse = true; }
		void SetPosition(const glm::vec3& pos) { _Position = pos; }
		void SetRotation(const glm::vec3& rot) { _Direction = rot; }
		void SetForward(const glm::vec3& forward) { _Forward = forward; }
		glm::mat4 GetView() const { return _ViewMatrix; }
		glm::mat4 GetViewProjection() const { return _ViewMatrix * _ProjectionMatrix; }
		glm::mat4 GetProjection() const { return _ProjectionMatrix; }
		glm::vec3 GetPosition() const { return _Position; }
		glm::vec3 GetForward() const { return _Forward; }
		glm::vec3 GetRight() const { return _Right; }
		void CalculateCameraMatrixes();
	protected:
		void AddPitch(float value);
		void AddYaw(float value);

		bool _FirstMouse = false;
		glm::mat4 _ViewMatrix = glm::mat4(1.0f);
		glm::mat4 _ProjectionMatrix = glm::mat4(1.0f);
		glm::vec3 _Position = glm::vec3(0.0f, 0.0f, 3.0f);
		glm::vec3 _Direction = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 _Up = glm::vec3(0.0, -1.0f, 0.0f);
		glm::vec3 _Forward = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 _Right = glm::vec3(0.0f, 0.0f, 0.0f);
	};
}