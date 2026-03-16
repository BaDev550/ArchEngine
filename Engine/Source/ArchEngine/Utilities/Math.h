#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <assimp/vector3.h>
#include <assimp/vector2.h>
#include <assimp/matrix4x4.h>
#include <Jolt/Jolt.h>
#include <Jolt/Math/Vec3.h>
#include <Jolt/Renderer/DebugRenderer.h>

namespace ae::math {
	constexpr float PI = 3.14;
	constexpr float TWO_PI = 2.0f * PI;
	constexpr float HALF_PI = PI / 2.0f;

	inline static glm::vec3 AssimpToGlm(const aiVector3D& vec) { return glm::vec3(vec.x, vec.y, vec.z); }
	inline static aiVector3D GlmToAssimp(const glm::vec3& vec) { return aiVector3D(vec.x, vec.y, vec.z); }
	inline static glm::vec2 AssimpToGlm(const aiVector2D& vec) { return glm::vec2(vec.x, vec.y); }
	inline static aiVector2D GlmToAssimp(const glm::vec2& vec) { return aiVector2D(vec.x, vec.y); }
	inline static glm::mat4 AssimpToGlm(const aiMatrix4x4& mat) {
		return glm::mat4(
			mat.a1, mat.b1, mat.c1, mat.d1,
			mat.a2, mat.b2, mat.c2, mat.d2,
			mat.a3, mat.b3, mat.c3, mat.d3,
			mat.a4, mat.b4, mat.c4, mat.d4
		);
	}

	inline static glm::vec3 JoltToGLM(JPH::Color c) { return glm::vec3(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f); }
	inline static glm::vec3 JoltToGLM(const JPH::RVec3& vec) { return glm::vec3(vec.GetX(), vec.GetY(), vec.GetZ()); }
	inline static glm::vec3 JoltToGlm(const JPH::Vec3& vec) { return glm::vec3(vec.GetX(), vec.GetY(), vec.GetZ()); }
	inline static glm::quat JoltToGlm(const JPH::Quat& q) { return glm::quat(q.GetW(), q.GetX(), q.GetY(), q.GetZ()); }
	inline static JPH::Vec3 GlmToJolt(const glm::vec3& vec) { return JPH::Vec3(vec.x, vec.y, vec.z); }
	inline static JPH::Quat GlmToJolt(const glm::quat& q) { return JPH::Quat(q.x, q.y, q.z, q.w); }

	inline static bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale) {
		glm::vec3 skew;
		glm::vec4 perspective;

		return glm::decompose(transform, scale, rotation, translation, skew, perspective);
	}
}