#include "ArchPch.h"
#include "Animator.h"
#include "Model.h"
#include <glm/gtx/quaternion.hpp>

namespace ae::grapichs {
	Animator::Animator() {
		_currentTime = 0.0f;
		_finalBoneMatrices.reserve(MAX_BONES);
		for (int i = 0; i < MAX_BONES; i++) {
			_finalBoneMatrices.push_back(glm::mat4(1.0f));
		}
	}

	void Animator::PlayAnimation(memory::Ref<Animation> animation) {
		if (_currentAnimation == animation)
			return;
		_currentAnimation = animation;
		_currentTime = 0.0f;
	}

	void Animator::UpdateAnimation(float dt, memory::Ref<Skeleton> skeleton) {
		_deltaTime = dt;

		if (_currentAnimation && skeleton) {
			_currentTime += _currentAnimation->GetTicksPerSecond() * dt;
			_currentTime = fmod(_currentTime, _currentAnimation->GetDuration());

			if (_rootBoneName.empty()) {
				for (const auto& [name, bone] : skeleton->GetBones()) {
					if (bone.ParentName.empty()) {
						_rootBoneName = name;
						_globalInverseTransform = glm::inverse(bone.LocalTransform);
						break;
					}
				}
			}

			CalculateBoneTransform(_rootBoneName, glm::mat4(1.0f), skeleton);
		}
	}

	void Animator::CalculateBoneTransform(const std::string& boneName, const glm::mat4& parentTransform, memory::Ref<Skeleton> skeleton) {
		if (!skeleton->HasBone(boneName)) return;

		Bone& bone = skeleton->GetBone(boneName);
		glm::mat4 nodeTransform = bone.LocalTransform;

		const AnimationChannel* channel = FindChannel(boneName);
		if (channel) {
			glm::vec3 position = InterpolatePosition(_currentTime, *channel);
			glm::quat rotation = InterpolateRotation(_currentTime, *channel);
			glm::vec3 scale = InterpolateScale(_currentTime, *channel);

			glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), position);
			glm::mat4 rotationMat = glm::toMat4(rotation);
			glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);

			nodeTransform = translationMat * rotationMat * scaleMat;
		}

		glm::mat4 globalTransform = parentTransform * nodeTransform;

		if (bone.ID != -1) {
			_finalBoneMatrices[bone.ID] = _globalInverseTransform * globalTransform * bone.OffsetMatrix;
		}

		for (const std::string& childName : bone.Children) {
			CalculateBoneTransform(childName, globalTransform, skeleton);
		}
	}

	const AnimationChannel* Animator::FindChannel(const std::string& boneName) {
		for (const auto& channel : _currentAnimation->GetChannels()) {
			if (channel.BoneName == boneName) {
				return &channel;
			}
		}
		return nullptr;
	}

	glm::vec3 Animator::InterpolatePosition(float animationTime, const AnimationChannel& channel) {
		if (channel.Position.size() == 1) return channel.Position[0].Value;

		int p0Index = -1, p1Index = -1;
		for (size_t i = 0; i < channel.Position.size() - 1; ++i) {
			if (animationTime < channel.Position[i + 1].Time) {
				p0Index = i; p1Index = i + 1; break;
			}
		}
		if (p0Index == -1) return channel.Position.back().Value;

		float scaleFactor = (animationTime - channel.Position[p0Index].Time) /
			(channel.Position[p1Index].Time - channel.Position[p0Index].Time);
		return glm::mix(channel.Position[p0Index].Value, channel.Position[p1Index].Value, scaleFactor);
	}

	glm::quat Animator::InterpolateRotation(float animationTime, const AnimationChannel& channel) {
		if (channel.Rotation.size() == 1) return glm::normalize(channel.Rotation[0].Value);

		int p0Index = -1, p1Index = -1;
		for (size_t i = 0; i < channel.Rotation.size() - 1; ++i) {
			if (animationTime < channel.Rotation[i + 1].Time) {
				p0Index = i; p1Index = i + 1; break;
			}
		}
		if (p0Index == -1) return glm::normalize(channel.Rotation.back().Value);

		float scaleFactor = (animationTime - channel.Rotation[p0Index].Time) /
			(channel.Rotation[p1Index].Time - channel.Rotation[p0Index].Time);
		return glm::normalize(glm::slerp(channel.Rotation[p0Index].Value, channel.Rotation[p1Index].Value, scaleFactor));
	}

	glm::vec3 Animator::InterpolateScale(float animationTime, const AnimationChannel& channel) {
		if (channel.Scale.size() == 1) return channel.Scale[0].Value;

		int p0Index = -1, p1Index = -1;
		for (size_t i = 0; i < channel.Scale.size() - 1; ++i) {
			if (animationTime < channel.Scale[i + 1].Time) {
				p0Index = i; p1Index = i + 1; break;
			}
		}
		if (p0Index == -1) return channel.Scale.back().Value;

		float scaleFactor = (animationTime - channel.Scale[p0Index].Time) /
			(channel.Scale[p1Index].Time - channel.Scale[p0Index].Time);
		return glm::mix(channel.Scale[p0Index].Value, channel.Scale[p1Index].Value, scaleFactor);
	}
}