#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/scene.h>
#include "ArchEngine/Utilities/Math.h"

namespace ae::grapichs {
	struct Keyframe {
		float Time;
		glm::vec3 Value;
	};

	struct RotationKeyframe {
		float Time;
		glm::quat Value;
	};

	struct BoneAnimationTrack {
		std::string BoneName;
		std::vector<Keyframe> Position;
		std::vector<RotationKeyframe> Rotation;
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

	struct SkeletonNode {
		std::string Name;
		glm::mat4 Transform;
		std::vector<SkeletonNode> Childs;

		static void ReadHierarchyData(SkeletonNode& dest, const aiNode* node) {
			dest.Name = node->mName.data;
			dest.Transform = math::AssimpToGlm(node->mTransformation);
			dest.Childs.reserve(node->mNumChildren);
			Logger_app::info("Bone {}:", dest.Name);

			for (uint32_t i = 0; i < node->mNumChildren; i++) {
				grapichs::SkeletonNode childNode;
				ReadHierarchyData(childNode, node->mChildren[i]);
				dest.Childs.push_back(childNode);
				Logger_app::info("	Child {}:", childNode.Name);
			}
		}
	};
}