#pragma once
#include "ArchEngine/AssetManager/Asset.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace ae::grapichs {
	struct VectorKeyframe {
		float Time;
		glm::vec3 Value;
	};

	struct QuatKeyframe {
		float Time;
		glm::quat Value;
	};

	struct AnimationChannel {
		std::string BoneName;
		std::vector<VectorKeyframe> Position;
		std::vector<QuatKeyframe> Rotation;
		std::vector<VectorKeyframe> Scale;
	};

	class Animation : public Asset {
	public:
		Animation(const std::string& name, float duration, float ticksPerSecond) 
			: _name(name), _duration(duration), _ticksPerSecond(ticksPerSecond) {}

		float GetDuration() const { return _duration; }
		float GetTicksPerSecond() const { return _ticksPerSecond; }
		const std::string& GetName() const { return _name; }
		std::vector<AnimationChannel>& GetChannels() { return _channels; }
		static AssetType GetStaticAssetType() { return AssetType::Animation; }
		virtual AssetType GetAssetType() const override { return GetStaticAssetType(); }
	private:
		std::string _name;
		float _duration;
		float _ticksPerSecond;
		std::vector<AnimationChannel> _channels;
	};
}