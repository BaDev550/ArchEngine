#pragma once
#include <ArchEngine/Core/Memory.h>
#include <ArchEngine/AssetManager/Asset.h>
#include <iostream>
#include <vector>

#include "Buffer.h"
#include "Animation.h"
#include "Material.h"
#include "Skeleton.h"

namespace ae::grapichs {
#define MAX_BONE_INFLUENCE 4
#define MAX_BONES 256
	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;
		int BoneIDs[MAX_BONE_INFLUENCE] = {-1, -1, -1, -1};
		float BoneWeights[MAX_BONE_INFLUENCE] = {0.0f, 0.0f, 0.0f, 0.0f};
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
		memory::Ref<Skeleton>& GetSkeleton() { return _skeleton; }
		void CreateVertexBuffer();
		void CreateIndexBuffer();
		void SetSkeleton(const memory::Ref<Skeleton>& skeleton) { _skeleton = skeleton; }
		bool HasSkeleton() const { return _skeleton; }
		void AddAnimation(Animation* animation) { _animations.push_back(animation); }
		bool HasAnimations() const { return !_animations.empty(); }
		bool IsRigged() { return _isRigged; }
		void SetIsRigged(bool rigged) { _isRigged = rigged; }
		const std::vector<Animation*>& GetAnimations() const { return _animations; }
		static AssetType GetStaticAssetType() { return AssetType::MeshSource; }
		virtual AssetType GetAssetType() const override { return GetStaticAssetType(); }
	private:
		std::filesystem::path _path;
		std::vector<Vertex> _vertices;
		std::vector<uint32_t> _indices;
		std::vector<Submesh> _submeshes;
		std::vector<AssetHandle> _materials;
		std::vector<Animation*> _animations;
		memory::Ref<Skeleton> _skeleton = nullptr;
		memory::Ref<Buffer> _vertexBuffer = nullptr;
		memory::Ref<Buffer> _indexBuffer = nullptr;
		bool _isRigged = false;
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
		SkeletalMesh(AssetHandle meshSource);
		memory::Ref<MaterialAsset>& GetMaterialByID(uint32_t id) { return _materials[id]; }
		memory::Ref<Buffer>& GetBonesBuffer() { return _bonesBuffer; }
		AssetHandle GetMeshSource() const { return _meshSource; }
		static AssetType GetStaticAssetType() { return AssetType::SkeletalMesh; }
		virtual AssetType GetAssetType() const override { return GetStaticAssetType(); }
	private:
		AssetHandle _meshSource = INVALID_ASSET_HANDLE;
		memory::Ref<Buffer> _bonesBuffer = nullptr;
		std::unordered_map<std::string, Bone> _bones;
		std::unordered_map<uint32_t, memory::Ref<MaterialAsset>> _materials;
	};
}