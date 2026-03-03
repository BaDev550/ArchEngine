#include <ArchPch.h>
#include "Model.h"
#include "Renderer.h"

namespace ae::grapichs {
	Model::Model(const std::string& path) {
		LoadModel(path);
	}

	void Model::LoadModel(const std::string& path)
	{
		Logger_renderer::info("Loading model: {}", path);

		PROFILE_SCOPE("Model Loading " + path);
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, MODEL_IMPORT_FLAGS);
		if (!scene) {
			Logger_renderer::warn(" -Failed to load mesh");
			return;
		}

		if (scene->HasMeshes()) {
			uint32_t vertexCount = 0;
			uint32_t indexCount = 0;

			_submeshes.reserve(scene->mNumMeshes);
			for (uint32_t m = 0; m < scene->mNumMeshes; m++) {
				aiMesh* mesh = scene->mMeshes[m];
				Submesh& submesh = _submeshes.emplace_back();
				submesh.VertexOffset = vertexCount;
				submesh.IndexOffset = indexCount;
				submesh.MaterialIndex = mesh->mMaterialIndex;
				submesh.VertexCount = mesh->mNumVertices;
				submesh.IndexCount = mesh->mNumFaces;
				submesh.Name = mesh->mName.C_Str();
				vertexCount += mesh->mNumVertices;
				indexCount += mesh->mNumFaces;
				for (size_t i = 0; i < mesh->mNumVertices; i++) {
					Vertex v;
					v.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
					if (mesh->HasNormals()) {
						v.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
					}
					if (mesh->HasTextureCoords(0)) {
						v.TexCoords = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
					}
					if (mesh->HasTangentsAndBitangents()) {
						v.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
						v.Bitangent = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
					}
					_vertices.push_back(v);
				}
				for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
					aiFace face = mesh->mFaces[i];
					for (uint32_t j = 0; j < face.mNumIndices; j++) {
						_indices.push_back(face.mIndices[j]);
					}
				}
			}
		}
		// Load Material

		// Create Buffers
		{
			uint64_t bufferSize = sizeof(Vertex) * _vertices.size();
			memory::Ref<Buffer> stagingBuffer = memory::Ref<Buffer>::Create(
				bufferSize, 
				vk::BufferUsageFlagBits::eTransferSrc, 
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
			);
			stagingBuffer->Map();
			stagingBuffer->Write(_vertices.data());
			stagingBuffer->Unmap();
			_vertexBuffer = memory::Ref<Buffer>::Create(
				bufferSize,
				vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
				vk::MemoryPropertyFlagBits::eDeviceLocal
			);
			Renderer::CopyBuffer(stagingBuffer, _vertexBuffer, bufferSize);
		}

		{
			uint64_t bufferSize = sizeof(uint32_t) * _indices.size();
			memory::Ref<Buffer> stagingBuffer = memory::Ref<Buffer>::Create(
				bufferSize,
				vk::BufferUsageFlagBits::eTransferSrc,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
			);
			stagingBuffer->Map();
			stagingBuffer->Write(_indices.data());
			stagingBuffer->Unmap();
			_indexBuffer = memory::Ref<Buffer>::Create(
				bufferSize,
				vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
				vk::MemoryPropertyFlagBits::eDeviceLocal
			);
			Renderer::CopyBuffer(stagingBuffer, _indexBuffer, bufferSize);
		}
		Logger_renderer::info(" -Loaded");
	}
}