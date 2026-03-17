#pragma once

#include "ArchEngine/AssetManager/Asset.h"
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>

namespace ae::grapichs {
	struct Bone {
		std::string Name;
		uint32_t ID;
		std::string ParentName = "";
		std::vector<std::string> Children;
		glm::mat4 LocalTransform;
		glm::mat4 OffsetMatrix;
	};

	class Skeleton : public Asset {
	public:
		Skeleton() = default;
		std::unordered_map<std::string, Bone>& GetBones() { return _bones; }
		const std::unordered_map<std::string, Bone>& GetBones() const { return _bones; }

		void AddBone(std::string name, Bone bone) { _bones[name] = bone; }
		bool HasBone(std::string name) const { return _bones.find(name) != _bones.end(); }
		Bone& GetBone(std::string name) { return _bones[name]; }
		uint32_t GetBoneCount() const { return static_cast<uint32_t>(_bones.size()); }

		static AssetType GetStaticAssetType() { return AssetType::Skeleton; }
		virtual AssetType GetAssetType() const override { return GetStaticAssetType(); }
	private:
		std::unordered_map<std::string, Bone> _bones;
	};
}