#include "ArchPch.h"
#include "AssetSerializer_Mesh.h"

#include "ArchEngine/Grapichs/Renderer.h"
#include "ArchEngine/AssetManager/AssetManager.h"
#include "ArchEngine/Utilities/Math.h"
#include "ArchEngine/Grapichs/Animation.h"

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

				if (mesh->HasBones()) {
					meshSource->SetSkinned(true);

					for (uint32_t i = 0; i < mesh->mNumBones; i++) {
						aiBone* bone = mesh->mBones[i];
						std::string boneName = bone->mName.C_Str();
						int boneID;

						if (!meshSource->HasBone(boneName)) {
							grapichs::Bone newBone{};
							newBone.Name = boneName;
							newBone.Index = meshSource->GetBoneCount();
							newBone.OffsetMatrix = math::AssimpToGlm(bone->mOffsetMatrix);

							boneID = meshSource->GetBoneCount();
							meshSource->AddBone(boneName, newBone);
						}
						else {
							boneID = meshSource->GetBone(boneName).Index;
						}

						for (uint32_t w = 0; w < bone->mNumWeights; w++) {
							uint32_t globalVertexId = submesh.VertexOffset + bone->mWeights[w].mVertexId;
							float weight = bone->mWeights[w].mWeight;
							grapichs::Vertex& vertexToEffect = meshSource->GetVertices()[globalVertexId];
							meshSource->AddBoneToVertex(vertexToEffect, boneID, weight);
						}
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

		if (meshSource->isSkinned()) {
			grapichs::SkeletonNode rootNode;
			grapichs::SkeletonNode::ReadHierarchyData(rootNode, scene->mRootNode);
			meshSource->SetRootNode(rootNode);
		}

		meshSource->CreateVertexBuffer();
		meshSource->CreateIndexBuffer();
		importer.FreeScene();
		return meshSource;
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

	void AssetSerializer_SkeletalMesh::Serialize(const AssetMetadata& metadata, const memory::Ref<Asset>& asset)
	{
		memory::Ref<grapichs::SkeletalMesh> mesh = asset.As<grapichs::SkeletalMesh>();

		std::string yamlString = SerializeToFile(mesh);
		std::filesystem::path serializePath = metadata.FilePath;
		std::ofstream fout(serializePath);

		if (!fout.is_open())
			return;

		fout << yamlString;
		fout.flush();
		fout.close();
	}

	bool AssetSerializer_SkeletalMesh::TryLoadData(const AssetMetadata& metadata, memory::Ref<Asset>& asset)
	{
		std::ifstream stream(metadata.FilePath);
		std::stringstream strStream;
		strStream << stream.rdbuf();
		memory::Ref<grapichs::SkeletalMesh> mesh;

		bool serialized = TryLoadFromFile(strStream.str(), mesh);
		if (!serialized)
			return false;

		mesh->SetAssetHandle(metadata.Handle);
		asset = mesh;
		return true;
	}

	std::string AssetSerializer_SkeletalMesh::SerializeToFile(memory::Ref<grapichs::SkeletalMesh>& mesh) const {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "SkeletalMesh";
		{
			out << YAML::BeginMap;
			out << YAML::Key << "MeshSource";
			out << YAML::Value << mesh->GetMeshSource();
			out << YAML::EndMap;
		}
		out << YAML::EndMap;

		return std::string(out.c_str());
	}

	bool AssetSerializer_SkeletalMesh::TryLoadFromFile(const std::string& filePath, memory::Ref<grapichs::SkeletalMesh>& mesh) {
		YAML::Node data = YAML::Load(filePath);
		if (!data["SkeletalMesh"])
			return false;

		YAML::Node rootNode = data["SkeletalMesh"];
		if (!rootNode["MeshSource"] && !rootNode["MeshAsset"])
			return false;

		AssetHandle meshSource = rootNode["MeshSource"].as<uint64_t>();
		mesh = memory::Ref<grapichs::SkeletalMesh>::Create(meshSource);
		return true;
	}
}