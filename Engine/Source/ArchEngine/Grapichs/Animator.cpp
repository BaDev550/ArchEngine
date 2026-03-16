#include "ArchPch.h"
#include "Animator.h"
#include <glm/gtx/quaternion.hpp>

namespace ae::grapichs {
	Animator::Animator(const std::vector<Animation*> animations, memory::Ref<SkeletalMesh>& skeletalMesh)
		: _animations(animations), _skeletalMesh(skeletalMesh), _currentTime(0.0f)
	{
		_finalBoneMatrices.resize(_skeletalMesh->GetBoneCount(), glm::mat4(1.0f));
	}

	void Animator::Update(float deltaTime) {
		if (_playingAnimation) {
			_currentTime += deltaTime;
			_currentTime = fmod(_currentTime, _playingAnimation->Duration);
			glm::mat4 matrix = glm::mat4(1.0f);
			CalculateBoneTransform(&_skeletalMesh->GetRootNode(), matrix);
		}
	}

	void Animator::PlayAnimation(const std::string& name)
	{
		Animation* anim = GetAnimation(name);
		if (anim && _playingAnimation != anim) {
			_playingAnimation = anim;
		}
	}

	void Animator::CalculateBoneTransform(const SkeletonNode* node, glm::mat4 parentTransform)
	{
		std::string nodeName = node->Name;
		glm::mat4 nodeTransform = node->Transform;

		const BoneAnimationTrack* track = nullptr;
		for (const auto& t : _playingAnimation->Tracks) {
			if (t.BoneName == nodeName) {
				track = &t;
				break;
			}
		}

		if (track) {
			glm::vec3 translation = InterpolatePosition(_currentTime, track);
			glm::quat rotation = InterpolateRotation(_currentTime, track);
			glm::vec3 scale = InterpolateScale(_currentTime, track);

			glm::mat4 transMat = glm::translate(glm::mat4(1.0f), translation);
			glm::mat4 rotMat   = glm::toMat4(rotation);
			glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);

			nodeTransform = transMat * rotMat * scaleMat;
		}

		glm::mat4 globalTransform = parentTransform * nodeTransform;

		if (_skeletalMesh->HasBone(nodeName)) {
			int index = _skeletalMesh->GetBone(nodeName).Index;
			glm::mat4 offset = _skeletalMesh->GetBone(nodeName).OffsetMatrix;
			_finalBoneMatrices[index] = globalTransform * offset;
		}

		for (const SkeletonNode& child : node->Childs) {
			CalculateBoneTransform(&child, globalTransform);
		}
	}

	glm::vec3 Animator::InterpolatePosition(float animationTime, const BoneAnimationTrack* track)
	{
		if (track->Position.size() == 1) return track->Position[0].Value;

		int p0Index = GetPositionIndex(animationTime, track);
		int p1Index = p0Index + 1;

		float deltaTime = track->Position[p1Index].Time - track->Position[p0Index].Time;
		float factor = (animationTime - track->Position[p0Index].Time) / deltaTime;

		return glm::mix(track->Position[p0Index].Value, track->Position[p1Index].Value, factor);
	}

	glm::quat Animator::InterpolateRotation(float animationTime, const BoneAnimationTrack* track)
	{
		if (track->Rotation.size() == 1) return glm::normalize(track->Rotation[0].Value);

		int p0Index = GetRotationIndex(animationTime, track);
		int p1Index = p0Index + 1;

		float deltaTime = track->Rotation[p1Index].Time - track->Rotation[p0Index].Time;
		float factor = (animationTime - track->Rotation[p0Index].Time) / deltaTime;

		glm::quat result = glm::slerp(track->Rotation[p0Index].Value, track->Rotation[p1Index].Value, factor);
		return glm::normalize(result);
	}

	glm::vec3 Animator::InterpolateScale(float animationTime, const BoneAnimationTrack* track)
	{
		if (track->Scale.size() == 1) return track->Scale[0].Value;

		int p0Index = GetScaleIndex(animationTime, track);
		int p1Index = p0Index + 1;

		float deltaTime = track->Scale[p1Index].Time - track->Scale[p0Index].Time;
		float factor = (animationTime - track->Scale[p0Index].Time) / deltaTime;

		return glm::mix(track->Scale[p0Index].Value, track->Scale[p1Index].Value, factor);
	}

	int Animator::GetPositionIndex(float animationTime, const BoneAnimationTrack* track)
	{
		for (int i = 0; i < track->Position.size() - 1; ++i) {
			if (animationTime < track->Position[i + 1].Time) return i;
		}
		return 0;
	}

	int Animator::GetRotationIndex(float animationTime, const BoneAnimationTrack* track)
	{
		for (int i = 0; i < track->Rotation.size() - 1; ++i) {
			if (animationTime < track->Rotation[i + 1].Time) return i;
		}
		return 0;
	}

	int Animator::GetScaleIndex(float animationTime, const BoneAnimationTrack* track)
	{
		for (int i = 0; i < track->Scale.size() - 1; ++i) {
			if (animationTime < track->Scale[i + 1].Time) return i;
		}
		return 0;
	}

	Animation* Animator::GetAnimation(const std::string& name)
	{
		for (Animation* anim : _animations) {
			if (anim->Name == name)
				return anim;
		}
		return nullptr;
	}
}