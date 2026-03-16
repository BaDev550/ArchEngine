#pragma once
#include <ArchEngine/Core/Memory.h>
#include <ArchEngine/AssetManager/Asset.h>
#include <iostream>
#include <vector>

#include "Buffer.h"
#include "Animation.h"
#include "Material.h"

namespace ae::grapichs {
#define MAX_BONE_INFLUENCE 4
#define MAX_BONES 256
	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;
		glm::ivec4 BoneIDs = { -1, -1, -1, -1 };
		glm::vec4 BoneWeights = { 0.0f, 0.0f, 0.0f, 0.0f };
	};

	struct Submesh {
		std::string Name;
		uint32_t VertexOffset;
		uint32_t VertexCount;
		uint32_t IndexOffset;
		uint32_t IndexCount;
		uint32_t MaterialIndex;
	};

	class MeshSource : public Asset {
	public:
		std::vector<Submesh>& GetSubmeshes() { return _submeshes; }
		std::vector<AssetHandle>& GetMaterials() { return _materials; }
		std::vector<Vertex>& GetVertices() { return _vertices; }
		std::vector<uint32_t>& GetIndices() { return _indices; }
		memory::Ref<Buffer>& GetVertexBuffer() { return _vertexBuffer; }
		memory::Ref<Buffer>& GetIndexBuffer() { return _indexBuffer; }
		std::unordered_map<std::string, Bone>& GetBones() { return _bones; }
		Bone& GetBone(std::string name) { return _bones[name]; }
		bool HasBone(std::string name) const;
		uint32_t GetBoneCount() const { return _boneCount; }
		void SetSkinned(bool skinned) { _skinned = skinned; }
		void CreateVertexBuffer();
		void CreateIndexBuffer();
		void AddBone(std::string name, Bone bone);
		void AddBoneToVertex(Vertex& vertex, int boneID, float weight);
		static AssetType GetStaticAssetType() { return AssetType::MeshSource; }
		virtual AssetType GetAssetType() const override { return GetStaticAssetType(); }
	private:
		std::filesystem::path _path;
		std::vector<Vertex> _vertices;
		std::vector<uint32_t> _indices;
		std::vector<Submesh> _submeshes;
		std::vector<AssetHandle> _materials;
		memory::Ref<Buffer> _vertexBuffer = nullptr;
		memory::Ref<Buffer> _indexBuffer = nullptr;
		std::unordered_map<std::string, Bone> _bones;
		uint32_t _boneCount;
		bool _skinned = false;
	};

	class StaticMesh : public Asset {
	public:
		StaticMesh(AssetHandle meshSource);
		~StaticMesh() = default;
		AssetHandle GetMeshSource() const { return _meshSource; }
		memory::Ref<MaterialAsset>& GetMaterialByID(uint32_t id) { return _materials[id]; }

		static AssetType GetStaticAssetType() { return AssetType::StaticMesh; }
		virtual AssetType GetAssetType() const override { return GetStaticAssetType(); }
	private:
		AssetHandle _meshSource = INVALID_ASSET_HANDLE;
		std::unordered_map<uint32_t, memory::Ref<MaterialAsset>> _materials;
	};

	class SkeletalMesh : public Asset {
	public:

		static AssetType GetStaticAssetType() { return AssetType::SkeletalMesh; }
		virtual AssetType GetAssetType() const override { return GetStaticAssetType(); }
	private:
		AssetHandle _meshSource = INVALID_ASSET_HANDLE;
		std::vector<Bone> _bones;
	};
}