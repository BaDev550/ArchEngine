#pragma once
#include <ArchEngine/Core/Memory.h>
#include <iostream>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/types.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>

#include "Buffer.h"

namespace ae::grapichs {
	constexpr uint32_t MODEL_IMPORT_FLAGS = 
		aiProcess_Triangulate | 
		aiProcess_FlipUVs | 
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace;

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

	class Model : public memory::RefCounted {
	public:
		Model(const std::string& path);
		~Model() = default;

		const std::vector<Submesh>& GetSubmeshes() const { return _submeshes; }
		const memory::Ref<Buffer>& GetVertexBuffer() const { return _vertexBuffer; }
		const memory::Ref<Buffer>& GetIndexBuffer() const { return _indexBuffer; }
	private:
		void LoadModel(const std::string& path);
		
		std::vector<Vertex> _vertices;
		std::vector<uint32_t> _indices;
		std::vector<Submesh> _submeshes;
		memory::Ref<Buffer> _vertexBuffer = nullptr;
		memory::Ref<Buffer> _indexBuffer = nullptr;
	};
}