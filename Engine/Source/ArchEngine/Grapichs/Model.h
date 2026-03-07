#pragma once
#include <ArchEngine/Core/Memory.h>
#include <ArchEngine/AssetManager/Asset.h>
#include <iostream>
#include <vector>

#include "Buffer.h"
#include "Material.h"

namespace ae::grapichs {
	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;
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
		void CreateVertexBuffer();
		void CreateIndexBuffer();
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
}