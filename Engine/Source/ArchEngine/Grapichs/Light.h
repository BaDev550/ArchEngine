#pragma once

// Forked from TAGE
#include <glm/glm.hpp>
#define MAX_POINT_LIGHTS 1000
#define MAX_SPOT_LIGHTS 1024

namespace ae::grapichs {
	struct DirectionalLight {
		glm::vec3 Direction;
		glm::vec3 Color;
		float Intensity;
		bool CastsShadows;

		DirectionalLight() : Direction(0.0f, -1.0f, 0.0f), Color(1.0f), Intensity(1.0f), CastsShadows(false) {}
	};

	struct alignas(16) PointLight {
		glm::vec3 Position;
		float pad0{};
		glm::vec3 Color;
		float Intensity;
		float Radius;
		float Falloff;
		int CastsShadows;
		float pad1{};

		PointLight() : Position(0.0f), Color(1.0f), Intensity(1.0f), Radius(1.0f), Falloff(1.0f), CastsShadows(false) {}
	};

	struct alignas(16) SpotLight {
		glm::vec3 Position;  float pad0{};
		glm::vec3 Direction;
		glm::vec3 Color;
		float Intensity;
		float Range;
		float Falloff;
		int CastsShadows;
		float pad1{};

		SpotLight() : Position(0.0f), Direction(0.0f, -1.0f, 0.0f), Color(1.0f), Intensity(1.0f), Range(1.0f), Falloff(1.0f), CastsShadows(false) {}
	};

	struct UniformBufferDirectionalLight
	{
		glm::vec3 Direction;
		glm::vec3 Color;
		float Intensity;
	};

	struct alignas(16) UniformBufferPointLights
	{
		uint32_t Count{ 0 };
		glm::vec3 Padding{ 0.0f };
		PointLight PointLights[MAX_POINT_LIGHTS]{};
	};

	struct alignas(16) UniformBufferSpotLights
	{
		uint32_t Count{ 0 };
		glm::vec3 Padding{ 0.0f };
		SpotLight SpotLights[MAX_SPOT_LIGHTS]{};
	};
}
