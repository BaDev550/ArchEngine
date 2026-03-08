#include "ArchPch.h"
#include "DebugRenderer.h"
#include "ArchEngine/Core/Application.h"
#include "Renderer.h"
#include "RenderAPI.h"

namespace ae::grapichs::debug {
	struct DebugRenderData {
		std::vector<LineDrawCommand> WaitingLineCommands;
		std::vector<TriangleCommand> WaitingTriangleCommands;
	} static s_data;

	void DebugRenderer::Init() {
		s_data.WaitingLineCommands.clear();
		s_data.WaitingTriangleCommands.clear();
	}

	void DebugRenderer::Destroy() {
		s_data.WaitingLineCommands.clear();
		s_data.WaitingTriangleCommands.clear();
	}

	void DebugRenderer::Update(float deltaTime) {
		for (auto it = s_data.WaitingLineCommands.begin(); it != s_data.WaitingLineCommands.end(); ) {
			if (it->Duration <= 0.0f) {
				it = s_data.WaitingLineCommands.erase(it);
			}
			else {
				it->Duration -= deltaTime;
				++it;
			}
		}
	}

	std::vector<LineDrawCommand>& DebugRenderer::GetLineDrawCommands() { return s_data.WaitingLineCommands; }

	void DebugRenderer::DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color, float duration) {
		s_data.WaitingLineCommands.emplace_back(start, end, color, duration);
	}

	void DebugRenderer::DrawAABB(const glm::vec3& minBounds, const glm::vec3& maxBounds, const glm::vec3& color, float duration) {
		glm::vec3 p0 = { minBounds.x, minBounds.y, minBounds.z };
		glm::vec3 p1 = { maxBounds.x, minBounds.y, minBounds.z };
		glm::vec3 p2 = { maxBounds.x, maxBounds.y, minBounds.z };
		glm::vec3 p3 = { minBounds.x, maxBounds.y, minBounds.z };

		glm::vec3 p4 = { minBounds.x, minBounds.y, maxBounds.z };
		glm::vec3 p5 = { maxBounds.x, minBounds.y, maxBounds.z };
		glm::vec3 p6 = { maxBounds.x, maxBounds.y, maxBounds.z };
		glm::vec3 p7 = { minBounds.x, maxBounds.y, maxBounds.z };

		DrawLine(p0, p1, color, duration);
		DrawLine(p1, p2, color, duration);
		DrawLine(p2, p3, color, duration);
		DrawLine(p3, p0, color, duration);
		DrawLine(p4, p5, color, duration);
		DrawLine(p5, p6, color, duration);
		DrawLine(p6, p7, color, duration);
		DrawLine(p7, p4, color, duration);
		DrawLine(p0, p4, color, duration);
		DrawLine(p1, p5, color, duration);
		DrawLine(p2, p6, color, duration);
		DrawLine(p3, p7, color, duration);
	}

	void DebugRenderer::DrawTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& color, float duration) {
		s_data.WaitingTriangleCommands.push_back(TriangleCommand(v0, v1, v2, color, duration));
	}
}