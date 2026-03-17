#pragma once 
#include <map>
#include <filesystem>
#include "Asset.h"

namespace ae {
	static std::map<std::filesystem::path, AssetType> s_ExtensionAssetMap = {
		{ ".scene", AssetType::Scene },
		{ ".png", AssetType::Texture },
		{ ".jpg", AssetType::Texture },
		{ ".jpeg", AssetType::Texture },
		{ ".tga", AssetType::Texture },
		{ ".bmp", AssetType::Texture },
		{ ".hdr", AssetType::EnviromentMap },

		{ ".fbx", AssetType::MeshSource },
		{ ".obj", AssetType::MeshSource },
		{ ".gltf", AssetType::MeshSource },
		{ ".dae", AssetType::MeshSource },

		{ ".mesh", AssetType::StaticMesh },
		{ ".skmesh", AssetType::SkeletalMesh },
		{ ".skeleton", AssetType::Skeleton },
		{ ".mat", AssetType::Material }
	};
}