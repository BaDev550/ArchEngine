#include "ArchPch.h"
#include "FreeCamera.h"
#include "ArchEngine/Core/Input.h"

namespace ae::grapichs {
	void FreeCamera::Update(float dt)
	{
		glm::vec3 forward = glm::normalize(_Forward);
		glm::vec3 right = glm::normalize(_Right);
		glm::vec3 position = _Position;

		float speed = 5.0f;
		if (Input::IsKeyPressed(key::LeftShift))
			speed *= 2.5f;
		
		if (Input::IsKeyPressed(key::W))
			position += forward * speed * dt;
		if (Input::IsKeyPressed(key::S))
			position -= forward * speed * dt;
		if (Input::IsKeyPressed(key::A))
			position -= right * speed * dt;
		if (Input::IsKeyPressed(key::D))
			position += right * speed * dt;
		if (Input::IsKeyPressed(key::E))
			position.y += speed * dt;
		if (Input::IsKeyPressed(key::Q))
			position.y -= speed * dt;

		_Position = position;

		if (_proccessingMouse) {
			double xpos, ypos;
			xpos = -Input::GetMousePosition().x;
			ypos = Input::GetMousePosition().y;

			if (_FirstMouse) {
				_lastX = xpos;
				_lastY = ypos;
				_FirstMouse = false;
			}

			float xoffset = static_cast<float>(xpos - _lastX);
			float yoffset = static_cast<float>(_lastY - ypos);

			_lastX = xpos;
			_lastY = ypos;

			xoffset *= 1.0f * dt;
			yoffset *= 1.0f * dt;

			Orbit(yoffset, xoffset);
		}

		CalculateCameraMatrixes();
	}
}