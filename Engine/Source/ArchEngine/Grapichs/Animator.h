#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "ArchEngine/Grapichs/Animation.h"
#include "ArchEngine/Grapichs/Skeleton.h"
#include "ArchEngine/Core/Memory.h"
#include <glm/glm.hpp>
#include <vector>

namespace ae::grapichs {
	class Animator : public memory::RefCounted {
	public:
		Animator();

		void PlayAnimation(memory::Ref<Animation> animation);
		void UpdateAnimation(float dt, memory::Ref<Skeleton> skeleton);

		const std::vector<glm::mat4>& GetFinalBoneMatrices() const { return _finalBoneMatrices; }
	private:
		void CalculateBoneTransform(const std::string& boneName, const glm::mat4& parentTransform, memory::Ref<Skeleton> skeleton);

		glm::vec3 InterpolatePosition(float animationTime, const AnimationChannel& channel);
		glm::quat InterpolateRotation(float animationTime, const AnimationChannel& channel);
		glm::vec3 InterpolateScale(float animationTime, const AnimationChannel& channel);

		const AnimationChannel* FindChannel(const std::string& boneName);

		glm::mat4 _globalInverseTransform;
		std::vector<glm::mat4> _finalBoneMatrices;
		memory::Ref<Animation> _currentAnimation;
		float _currentTime;
		float _deltaTime;
		std::string _rootBoneName;
	};

}