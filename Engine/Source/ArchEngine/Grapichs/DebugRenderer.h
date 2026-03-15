#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace ae::grapichs::debug {
	struct DebugVertex {
		glm::vec3 Position;
		glm::vec3 Color;
	};

	struct LineDrawCommand {
		glm::vec3 Start;
		glm::vec3 End;
		glm::vec3 Color;
		float Duration;
	};

	struct TriangleCommand  {
		glm::vec3 V0, V1, V2;
		glm::vec3 Color;
		float Duration;
	};

	class DebugRenderer {
	public:
		static void Init();
		static void Destroy();
		static void Update(float deltaTime);
		
		static std::vector<LineDrawCommand>& GetLineDrawCommands();
		static void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color = glm::vec3(1.0f), float duration = 0.0f);
		static void DrawAABB(const glm::vec3& minBounds, const glm::vec3& maxBounds, const glm::vec3& color = glm::vec3(0.0f, 1.0f, 0.0f), float duration = 0.0f);
		static void DrawTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& color, float duration = 0.0f);
	};
}