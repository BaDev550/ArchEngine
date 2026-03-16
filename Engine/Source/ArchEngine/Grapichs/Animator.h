#pragma once
#include <vector>
#include "ArchEngine/Core/Memory.h"
#include "Animation.h"
#include "Model.h"

namespace ae::grapichs {
	class Animator {
	public:
		Animator(const std::vector<Animation*> animations, memory::Ref<SkeletalMesh>& skeletalMesh);
		void Update(float deltaTime);
		void PlayAnimation(const std::string& name);
		const std::vector<glm::mat4>& GetFinalBoneMatrices() const { return _finalBoneMatrices; };
	private:
		void CalculateBoneTransform(const SkeletonNode* node, glm::mat4 parentTransform);
		glm::vec3 InterpolatePosition(float animationTime, const BoneAnimationTrack* track);
		glm::quat InterpolateRotation(float animationTime, const BoneAnimationTrack* track);
		glm::vec3 InterpolateScale(float animationTime, const BoneAnimationTrack* track);
		int GetPositionIndex(float animationTime, const BoneAnimationTrack* track);
		int GetRotationIndex(float animationTime, const BoneAnimationTrack* track);
		int GetScaleIndex(float animationTime, const BoneAnimationTrack* track);
		Animation* GetAnimation(const std::string& name);
		std::vector<glm::mat4> _finalBoneMatrices;
		std::vector<Animation*> _animations;
		Animation* _playingAnimation;
		memory::Ref<SkeletalMesh>& _skeletalMesh;
		float _currentTime;
	};
}