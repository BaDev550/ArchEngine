#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace ae::grapichs {
	struct Keyframe {
		float Time;
		glm::vec3 Value;
	};

	struct BoneAnimationTrack {
		int BoneIndex;
		std::vector<Keyframe> Position;
		std::vector<Keyframe> Rotation;
		std::vector<Keyframe> Scale;
	};

	struct Animation {
		std::string Name;
		float Duration;
		std::vector<BoneAnimationTrack> Tracks;
	};

	struct Bone {
		std::string Name;
		glm::mat4 OffsetMatrix;
		int Index;
	};
}