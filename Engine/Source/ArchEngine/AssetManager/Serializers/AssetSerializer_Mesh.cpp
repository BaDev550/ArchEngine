#include "ArchPch.h"
#include "AssetSerializer_Mesh.h"

#include "ArchEngine/Grapichs/Renderer.h"
#include "ArchEngine/AssetManager/AssetManager.h"
#include "ArchEngine/Utilities/Math.h"

#include <yaml-cpp/yaml.h>

namespace ae {
	// SOURCE MESH IMPORTER
	MeshSourceImporter::MeshSourceImporter(const std::filesystem::path& path) : _path(path) {}
	memory::Ref<grapichs::MeshSource> MeshSourceImporter::Import() {
		memory::Ref<grapichs::MeshSource> meshSource = memory::Ref<grapichs::MeshSource>::Create();
		Logger_app::info("Loading mesh source: {}", _path.string());

		PROFILE_SCOPE("Source mesh loading: " + _path.string());
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(_path.string(), MODEL_IMPORT_FLAGS);
		if (!scene) {
			Logger_renderer::warn(" -Failed to load mesh");
		}
		LoadData(meshSource, scene);
		importer.FreeScene();
		return meshSource;
	}

	memory::Ref<grapichs::MeshSource> MeshSourceImporter::ImportFromMemory(const uint8_t* data, size_t size)
	{
		memory::Ref<grapichs::MeshSource> meshSource = memory::Ref<grapichs::MeshSource>::Create();
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFileFromMemory(data, size, MODEL_IMPORT_FLAGS);
		if (!scene) {
			Logger_renderer::error("Failed to load mesh from pack buffer");
			return nullptr;
		}
		LoadData(meshSource, scene);
		importer.FreeScene();
		return meshSource;
	}

	void MeshSourceImporter::LoadData(memory::Ref<grapichs::MeshSource>& meshSource, const aiScene* scene)
	{
		if (scene->HasMeshes()) {
			uint32_t vertexCount = 0;
			uint32_t indexCount = 0;
			uint32_t totalVertices = 0;
			uint32_t totalIndices = 0;
			for (uint32_t m = 0; m < scene->mNumMeshes; m++) {
				totalVertices += scene->mMeshes[m]->mNumVertices;
				totalIndices += scene->mMeshes[m]->mNumFaces * 3;
			}

			meshSource->GetSubmeshes().reserve(scene->mNumMeshes);
			meshSource->GetVertices().reserve(totalVertices);
			meshSource->GetIndices().reserve(totalIndices);
			for (uint32_t m = 0; m < scene->mNumMeshes; m++) {
				aiMesh* mesh = scene->mMeshes[m];
				grapichs::Submesh& submesh = meshSource->GetSubmeshes().emplace_back();
				submesh.VertexOffset = vertexCount;
				submesh.IndexOffset = indexCount;
				submesh.MaterialIndex = mesh->mMaterialIndex;
				submesh.VertexCount = mesh->mNumVertices;
				submesh.IndexCount = mesh->mNumFaces * 3;
				submesh.Name = mesh->mName.C_Str();
				vertexCount += mesh->mNumVertices;
				indexCount += submesh.IndexCount;
				for (size_t i = 0; i < mesh->mNumVertices; i++) {
					grapichs::Vertex v;
					v.Position = math::AssimpToGlm(mesh->mVertices[i]);
					v.Normal = math::AssimpToGlm(mesh->mNormals[i]);
					if (mesh->HasTextureCoords(0)) {
						v.TexCoords = math::AssimpToGlm(mesh->mTextureCoords[0][i]);
					}
					if (mesh->HasTangentsAndBitangents()) {
						v.Tangent = math::AssimpToGlm(mesh->mTangents[i]);
						v.Bitangent = math::AssimpToGlm(mesh->mBitangents[i]);
					}
					meshSource->GetVertices().push_back(v);
				}
				for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
					aiFace face = mesh->mFaces[i];
					for (uint32_t j = 0; j < face.mNumIndices; j++) {
						meshSource->GetIndices().push_back(face.mIndices[j]);
					}
				}
			}
		}

		// Load Material
		if (scene->HasMaterials()) {
			memory::Ref<grapichs::Texture2D>& whiteTexture = grapichs::Renderer::GetWhiteTexture();

			meshSource->GetMaterials().resize(scene->mNumMaterials);
			for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
				aiMaterial* aiMat = scene->mMaterials[i];
				auto aiMaterialName = aiMat->GetName();
				auto modelDirectory = _path.parent_path();
				aiString aiTexturePath;
				memory::Ref<grapichs::Material> mat = memory::Ref<grapichs::Material>::Create(grapichs::Renderer::GetShaderLibrary().GetShader("ForwardShader"));
				memory::Ref<grapichs::MaterialAsset> matAsset = memory::Ref<grapichs::MaterialAsset>::Create(mat);

				bool hasAlbedo = aiMat->GetTexture(AI_MATKEY_BASE_COLOR_TEXTURE, &aiTexturePath) == AI_SUCCESS;
				if (!hasAlbedo)
					hasAlbedo = aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexturePath) == AI_SUCCESS;

				if (hasAlbedo) {
					grapichs::TextureSpecification specs{};
					specs.Attachment = false;
					specs.GenerateMipMap = false;
					specs.Filter = vk::Filter::eLinear;
					specs.Wrap = vk::SamplerAddressMode::eClampToBorder;
					specs.Format = vk::Format::eR8G8B8A8Srgb;
					auto texturePath = modelDirectory / aiTexturePath.C_Str();
					AssetHandle texture = AssetManager::AddMemoryOnlyAsset(memory::Ref<grapichs::Texture2D>::Create(specs, texturePath.string()));
					matAsset->SetAlbedoTexture(texture);
				}
				else {
					matAsset->SetAlbedoTexture(whiteTexture->GetAssetHandle());
				}

				bool hasNormal = aiMat->GetTexture(aiTextureType_NORMALS, 0, &aiTexturePath) == AI_SUCCESS;
				if (hasNormal) {
					grapichs::TextureSpecification specs{};
					specs.Attachment = false;
					specs.GenerateMipMap = false;
					specs.Filter = vk::Filter::eLinear;
					specs.Wrap = vk::SamplerAddressMode::eClampToBorder;
					specs.Format = vk::Format::eR8G8B8A8Srgb;
					auto texturePath = modelDirectory / aiTexturePath.C_Str();
					AssetHandle texture = AssetManager::AddMemoryOnlyAsset(memory::Ref<grapichs::Texture2D>::Create(specs, texturePath.string()));
					matAsset->SetNormalTexture(texture);
				}
				AssetHandle materialHandle = AssetManager::AddMemoryOnlyAsset(matAsset);
				meshSource->GetMaterials()[i] = materialHandle;
			}
		}

		meshSource->CreateVertexBuffer();
		meshSource->CreateIndexBuffer();
	}

	// SOURCE MESH
	bool AssetSerializer_MeshSource::TryLoadData(const AssetMetadata& metadata, memory::Ref<Asset>& asset)
	{
		MeshSourceImporter importer(metadata.FilePath);
		memory::Ref<grapichs::MeshSource> meshSource = importer.Import();
		if (!meshSource)
			return false;
		asset = meshSource;
		asset->SetAssetHandle(metadata.Handle);
		return true;
	}

	bool AssetSerializer_MeshSource::TryLoadFromBuffer(const AssetHandle& handle, const std::vector<uint8_t>& buffer, memory::Ref<Asset>& asset)
	{
		MeshSourceImporter importer("");
		memory::Ref<grapichs::MeshSource> meshSource = importer.ImportFromMemory(buffer.data(), buffer.size());

		if (!meshSource) return false;

		asset = meshSource;
		asset->SetAssetHandle(handle);
		return true;
	}

	// STATIC MESH
	void AssetSerializer_StaticMesh::Serialize(const AssetMetadata& metadata, const memory::Ref<Asset>& asset)
	{
		memory::Ref<grapichs::StaticMesh> mesh = asset.As<grapichs::StaticMesh>();

		std::string yamlString = SerializeToFile(mesh);
		std::filesystem::path serializePath = metadata.FilePath;
		std::ofstream fout(serializePath);

		if (!fout.is_open())
			return;

		fout << yamlString;
		fout.flush();
		fout.close();
	}

	bool AssetSerializer_StaticMesh::TryLoadData(const AssetMetadata& metadata, memory::Ref<Asset>& asset)
	{
		std::ifstream stream(metadata.FilePath);
		std::stringstream strStream;
		strStream << stream.rdbuf();
		memory::Ref<grapichs::StaticMesh> mesh;

		bool serialized = TryLoadFromFile(strStream.str(), mesh);
		if (!serialized)
			return false;

		mesh->SetAssetHandle(metadata.Handle);
		asset = mesh;
		return true;
	}

	bool AssetSerializer_StaticMesh::TryLoadFromBuffer(const AssetHandle& handle, const std::vector<uint8_t>& buffer, memory::Ref<Asset>& asset)
	{
		std::string yamlContent(buffer.begin(), buffer.end());

		memory::Ref<grapichs::StaticMesh> mesh;
		if (!TryLoadFromFile(yamlContent, mesh))
			return false;

		mesh->SetAssetHandle(handle);
		asset = mesh;
		return true;
	}

	std::string AssetSerializer_StaticMesh::SerializeToFile(memory::Ref<grapichs::StaticMesh>& mesh) const
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Mesh";
		{
			out << YAML::BeginMap;
			out << YAML::Key << "MeshSource";
			out << YAML::Value << mesh->GetMeshSource();
			out << YAML::EndMap;
		}
		out << YAML::EndMap;

		return std::string(out.c_str());
	}

	bool AssetSerializer_StaticMesh::TryLoadFromFile(const std::string& filePath, memory::Ref<grapichs::StaticMesh>& mesh)
	{
		YAML::Node data = YAML::Load(filePath);
		if (!data["Mesh"])
			return false;

		YAML::Node rootNode = data["Mesh"];
		if (!rootNode["MeshSource"] && !rootNode["MeshAsset"])
			return false;

		AssetHandle meshSource = rootNode["MeshSource"].as<uint64_t>();
		mesh = memory::Ref<grapichs::StaticMesh>::Create(meshSource);
		return true;
	}
}