#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <assimp/vector3.h>
#include <assimp/vector2.h>
#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
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
	inline static glm::quat AssimpToGlm(const aiQuaternion& quat) { return glm::quat(quat.w, quat.x, quat.y, quat.z); }
	inline glm::mat4 AssimpToGlm(const aiMatrix4x4& from) {
		glm::mat4 to;
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
		return to;
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