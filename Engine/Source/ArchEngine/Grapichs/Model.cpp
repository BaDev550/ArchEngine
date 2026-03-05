#include <ArchPch.h>
#include "Model.h"
#include "Renderer.h"

namespace ae::grapichs {
	Model::Model(const std::string& path) : _path(path) {
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
				submesh.IndexCount = mesh->mNumFaces * 3;
				submesh.Name = mesh->mName.C_Str();
				vertexCount += mesh->mNumVertices;
				indexCount += submesh.IndexCount;
				for (size_t i = 0; i < mesh->mNumVertices; i++) {
					Vertex v;
					v.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
					v.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
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
		if (scene->HasMaterials()) {
			memory::Ref<Texture2D>& whiteTexture = Renderer::GetWhiteTexture();

			_materials.resize(scene->mNumMaterials);
			for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
				aiMaterial* aiMat = scene->mMaterials[i];
				auto aiMaterialName = aiMat->GetName();
				auto modelDirectory = _path.parent_path();
				aiString aiTexturePath;
				memory::Ref<Material> mat = memory::Ref<Material>::Create(Renderer::GetShaderLibrary().GetShader("ForwardShader"));

				bool hasAlbedo = aiMat->GetTexture(AI_MATKEY_BASE_COLOR_TEXTURE, &aiTexturePath) == AI_SUCCESS;
				if (!hasAlbedo)
					hasAlbedo = aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexturePath) == AI_SUCCESS;

				if (hasAlbedo) {
					TextureSpecification specs{};
					specs.Attachment = false;
					specs.GenerateMipMap = false;
					specs.Filter = vk::Filter::eLinear;
					specs.Wrap = vk::SamplerAddressMode::eClampToBorder;
					specs.Format = vk::Format::eR8G8B8A8Unorm;
					auto texturePath = modelDirectory / aiTexturePath.C_Str();
					memory::Ref<Texture2D> texture = memory::Ref<Texture2D>::Create(specs, texturePath.string());
					mat->SetAlbedoTexture(texture);
				}

				bool hasNormal = aiMat->GetTexture(aiTextureType_NORMALS, 0, &aiTexturePath) == AI_SUCCESS;
				if (hasNormal) {
					TextureSpecification specs{};
					specs.Attachment = false;
					specs.GenerateMipMap = false;
					specs.Filter = vk::Filter::eLinear;
					specs.Wrap = vk::SamplerAddressMode::eClampToBorder;
					specs.Format = vk::Format::eR8G8B8A8Unorm;
					auto texturePath = modelDirectory / aiTexturePath.C_Str();
					memory::Ref<Texture2D> texture = memory::Ref<Texture2D>::Create(specs, texturePath.string());
					mat->SetNormalTexture(texture);
				}
				mat->Build();

				_materials[i] = mat;
			}
		}

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
		importer.FreeScene();
		
		Logger_renderer::info(" -Loaded");
	}
}