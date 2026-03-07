#pragma once
#include "AssetSerializer.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/types.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <filesystem>

#include "ArchEngine/Grapichs/Model.h"

namespace ae {
	constexpr uint32_t MODEL_IMPORT_FLAGS =
		aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_GenSmoothNormals |
		aiProcess_GlobalScale |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ImproveCacheLocality |
		aiProcess_CalcTangentSpace;

	class MeshSourceImporter {
	public:
		MeshSourceImporter(const std::filesystem::path& path);
		memory::Ref<grapichs::MeshSource> Import();
	private:
		const std::filesystem::path _path;
	};

	class AssetSerializer_MeshSource : public AssetSerializer {
	public:
		virtual void Serialize(const AssetMetadata& metadata, const memory::Ref<Asset>& asset) override {}
		virtual bool TryLoadData(const AssetMetadata& metadata, memory::Ref<Asset>& asset) override;
	};

	class AssetSerializer_StaticMesh : public AssetSerializer {
	public:
		virtual void Serialize(const AssetMetadata& metadata, const memory::Ref<Asset>& asset) override;
		virtual bool TryLoadData(const AssetMetadata& metadata, memory::Ref<Asset>& asset) override;
	private:
		std::string SerializeToFile(memory::Ref<grapichs::StaticMesh>& mesh) const;
		bool TryLoadFromFile(const std::string& filePath, memory::Ref<grapichs::StaticMesh>& mesh);
	};
}