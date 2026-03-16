#pragma once
#include "ArchEngine/Utilities/UUID.h"
#include "ArchEngine/Core/Memory.h"

namespace ae {
#define INVALID_ASSET_HANDLE 0
#define INVALID_ASSET_TYPE AssetType::Unknown
	enum class AssetType : uint8_t {
		Unknown = 0,
		Texture,
		MeshSource,
		StaticMesh,
		SkeletalMesh,
		Animation,
		Material,
		EnviromentMap,
		Scene
	};

	static std::string AssetTypeToString(AssetType type) {
		switch (type) {
		case AssetType::Unknown: return "Unknown";
		case AssetType::Scene: return "Scene";
		case AssetType::Texture: return "Texture";
		case AssetType::EnviromentMap: return "EnviromentMap";
		case AssetType::MeshSource: return "MeshSource";
		case AssetType::StaticMesh: return "StaticMesh";
		case AssetType::SkeletalMesh: return "SkeletalMesh";
		case AssetType::Animation: return "Animation";
		case AssetType::Material: return "Material";
		default: return "Unknown";
		}
	}

	static AssetType StringToAssetType(const std::string& str) {
		if (str == "Unknown") return AssetType::Unknown;
		if (str == "Scene") return AssetType::Scene;
		if (str == "Texture") return AssetType::Texture;
		if (str == "EnviromentMap") return AssetType::EnviromentMap;
		if (str == "MeshSource") return AssetType::MeshSource;
		if (str == "StaticMesh") return AssetType::StaticMesh;
		if (str == "SkeletalMesh") return AssetType::SkeletalMesh;
		if (str == "Animation") return AssetType::Animation;
		if (str == "Material") return AssetType::Material;
		return INVALID_ASSET_TYPE;
	}

	using AssetHandle = UUID;
	class Asset : public memory::RefCounted {
	public:
		virtual ~Asset() = default;
		void SetAssetHandle(AssetHandle handle) { _AssetHandle = handle; }
		AssetHandle GetAssetHandle() const { return _AssetHandle; }
		static AssetType GetStaticAssetType() { return INVALID_ASSET_TYPE; }
		virtual AssetType GetAssetType() const { return GetStaticAssetType(); }
	protected:
		AssetHandle _AssetHandle = INVALID_ASSET_HANDLE;
	};
}