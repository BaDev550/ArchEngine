#include "ArchPch.h"
#include "AssetSerializer_Mesh.h"

#include "ArchEngine/Grapichs/Renderer.h"
#include "ArchEngine/AssetManager/AssetManager.h"
#include "ArchEngine/Utilities/Math.h"
#include "ArchEngine/Grapichs/Animation.h"

#include <yaml-cpp/yaml.h>

namespace ae {
	namespace utils {
		static void TraverseNode(const aiNode* node, const std::string& parentName, memory::Ref<grapichs::Skeleton>& skeleton, memory::Ref<grapichs::MeshSource>& meshSource) {
			grapichs::Bone newBoneNode;
			newBoneNode.Name = node->mName.C_Str();
			newBoneNode.ParentName = parentName;
			newBoneNode.LocalTransform = math::AssimpToGlm(node->mTransformation);

			if (skeleton->HasBone(newBoneNode.Name)) {
				newBoneNode.ID = skeleton->GetBone(newBoneNode.Name).ID;
				newBoneNode.OffsetMatrix = skeleton->GetBone(newBoneNode.Name).OffsetMatrix;
			}
			else {
				newBoneNode.ID = -1;
				newBoneNode.OffsetMatrix = glm::mat4(1.0f);
			}

			if (!parentName.empty() && skeleton->HasBone(parentName)) {
				skeleton->GetBone(parentName).Children.push_back(newBoneNode.Name);
			}

			skeleton->AddBone(newBoneNode.Name, newBoneNode);

			for (uint32_t i = 0; i < node->mNumChildren; i++) {
				TraverseNode(node->mChildren[i], newBoneNode.Name, skeleton, meshSource);
			}
		}
	}

	// SOURCE MESH IMPORTER
	MeshSourceImporter::MeshSourceImporter(const std::filesystem::path& path) : _path(path) {}
	memory::Ref<grapichs::MeshSource> MeshSourceImporter::Import() {
		memory::Ref<grapichs::MeshSource> meshSource = memory::Ref<grapichs::MeshSource>::Create();
		memory::Ref<grapichs::Skeleton> skeleton = memory::Ref<grapichs::Skeleton>::Create();
		uint32_t boneCount = 0;

		Logger_app::info("Loading mesh source: {}", _path.string());
		
		PROFILE_SCOPE("Source mesh loading: " + _path.string());
		std::filesystem::path meshDirectory = _path.parent_path();
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
					meshSource->SetIsRigged(true);
					for (uint32_t i = 0; i < mesh->mNumBones; i++) {
						aiBone* aiBone = mesh->mBones[i];
						std::string boneName = aiBone->mName.C_Str();
						uint32_t boneID;

						if (!skeleton->HasBone(boneName)) {
							grapichs::Bone newBone{};
							newBone.Name = boneName;
							newBone.ID = boneCount;
							newBone.OffsetMatrix = math::AssimpToGlm(aiBone->mOffsetMatrix);

							skeleton->AddBone(boneName, newBone);
							boneID = boneCount;
							boneCount++;
						}
						else {
							boneID = skeleton->GetBone(boneName).ID;
						}
						for (uint32_t w = 0; w < aiBone->mNumWeights; w++) {
							uint32_t vertexID = submesh.VertexOffset + aiBone->mWeights[w].mVertexId;
							float weight = aiBone->mWeights[w].mWeight;
							grapichs::Vertex& vertex = meshSource->GetVertices()[vertexID];
							for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
								if (vertex.BoneIDs[i] < 0) {
									vertex.BoneIDs[i] = boneID;
									vertex.BoneWeights[i] = weight;
									break;
								}
							}
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
					auto texturePath = meshDirectory / aiTexturePath.C_Str();
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
					auto texturePath = meshDirectory / aiTexturePath.C_Str();
					AssetHandle texture = AssetManager::AddMemoryOnlyAsset(memory::Ref<grapichs::Texture2D>::Create(specs, texturePath.string()));
					matAsset->SetNormalTexture(texture);
				}
				AssetHandle materialHandle = AssetManager::AddMemoryOnlyAsset(matAsset);
				meshSource->GetMaterials()[i] = materialHandle;
			}
		}

		if (meshSource->IsRigged()) {
			utils::TraverseNode(scene->mRootNode, "", skeleton, meshSource);
			AssetHandle skeletonHandle = AssetManager::AddMemoryOnlyAsset(skeleton);
			meshSource->SetSkeleton(skeleton);
			Logger_app::info("Imported Skeleton with {} nodes.", skeleton->GetBones().size());
		}

		if (scene->HasAnimations()) {
			for (uint32_t i = 0; i < scene->mNumAnimations; i++) {
				aiAnimation* aiAnim = scene->mAnimations[i];

				float ticksPerSecond = static_cast<float>(aiAnim->mTicksPerSecond != 0.0 ? aiAnim->mTicksPerSecond : 24.0f);

				memory::Ref<grapichs::Animation> animation = memory::Ref<grapichs::Animation>::Create(
					aiAnim->mName.C_Str(),
					static_cast<float>(aiAnim->mDuration),
					ticksPerSecond
				);

				for (uint32_t c = 0; c < aiAnim->mNumChannels; c++) {
					aiNodeAnim* aiChannel = aiAnim->mChannels[c];
					grapichs::AnimationChannel channel;
					channel.BoneName = aiChannel->mNodeName.C_Str();

					for (uint32_t p = 0; p < aiChannel->mNumPositionKeys; p++) {
						grapichs::VectorKeyframe key;
						key.Value = math::AssimpToGlm(aiChannel->mPositionKeys[p].mValue);
						key.Time = static_cast<float>(aiChannel->mPositionKeys[p].mTime);
						channel.Position.push_back(key);
					}

					for (uint32_t r = 0; r < aiChannel->mNumRotationKeys; r++) {
						grapichs::QuatKeyframe key;
						key.Value = math::AssimpToGlm(aiChannel->mRotationKeys[r].mValue);
						key.Time = static_cast<float>(aiChannel->mRotationKeys[r].mTime);
						channel.Rotation.push_back(key);
					}

					for (uint32_t s = 0; s < aiChannel->mNumScalingKeys; s++) {
						grapichs::VectorKeyframe key;
						key.Value = math::AssimpToGlm(aiChannel->mScalingKeys[s].mValue);
						key.Time = static_cast<float>(aiChannel->mScalingKeys[s].mTime);
						channel.Scale.push_back(key);
					}

					animation->GetChannels().push_back(channel);
				}

				AssetHandle animHandle = AssetManager::AddMemoryOnlyAsset(animation);
				meshSource->AddAnimation(animation.Get());
			}
			Logger_app::info("Imported {} animations.", scene->mNumAnimations);
		}

		meshSource->CreateVertexBuffer();
		meshSource->CreateIndexBuffer();
		importer.FreeScene();
		return meshSource;
	}

	AnimationImporter::AnimationImporter(const std::filesystem::path& path) : _path(path) {}
	memory::Ref<grapichs::Animation> AnimationImporter::Import() {
		Logger_app::info("Loading animation: {}", _path.string());

		PROFILE_SCOPE("Animation loading: " + _path.string());
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(_path.string(), aiProcess_GlobalScale);

		if (!scene || !scene->HasAnimations()) {
			Logger_renderer::warn(" - Failed to load animation or no animation found in file");
			return nullptr;
		}

		aiAnimation* aiAnim = scene->mAnimations[0];
		float ticksPerSecond = static_cast<float>(aiAnim->mTicksPerSecond != 0.0 ? aiAnim->mTicksPerSecond : 24.0f);

		memory::Ref<grapichs::Animation> animation = memory::Ref<grapichs::Animation>::Create(
			_path.stem().string(),
			static_cast<float>(aiAnim->mDuration),
			ticksPerSecond
		);

		for (uint32_t c = 0; c < aiAnim->mNumChannels; c++) {
			aiNodeAnim* aiChannel = aiAnim->mChannels[c];
			grapichs::AnimationChannel channel;
			channel.BoneName = aiChannel->mNodeName.C_Str();

			for (uint32_t p = 0; p < aiChannel->mNumPositionKeys; p++) {
				grapichs::VectorKeyframe key;
				key.Value = math::AssimpToGlm(aiChannel->mPositionKeys[p].mValue);
				key.Time = static_cast<float>(aiChannel->mPositionKeys[p].mTime);
				channel.Position.push_back(key);
			}

			for (uint32_t r = 0; r < aiChannel->mNumRotationKeys; r++) {
				grapichs::QuatKeyframe key;
				key.Value = math::AssimpToGlm(aiChannel->mRotationKeys[r].mValue);
				key.Time = static_cast<float>(aiChannel->mRotationKeys[r].mTime);
				channel.Rotation.push_back(key);
			}

			for (uint32_t s = 0; s < aiChannel->mNumScalingKeys; s++) {
				grapichs::VectorKeyframe key;
				key.Value = math::AssimpToGlm(aiChannel->mScalingKeys[s].mValue);
				key.Time = static_cast<float>(aiChannel->mScalingKeys[s].mTime);
				channel.Scale.push_back(key);
			}

			animation->GetChannels().push_back(channel);
		}

		importer.FreeScene();
		return animation;
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

	// SKELETAL MESH
	void AssetSerializer_SkeletalMesh::Serialize(const AssetMetadata& metadata, const memory::Ref<Asset>& asset){
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

	bool AssetSerializer_SkeletalMesh::TryLoadData(const AssetMetadata& metadata, memory::Ref<Asset>& asset) {
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

	bool AssetSerializer_SkeletalMesh::TryLoadFromFile(const std::string& filePath, memory::Ref<grapichs::SkeletalMesh>& mesh) {
		YAML::Node data = YAML::Load(filePath);
		if (!data["Mesh"])
			return false;

		YAML::Node rootNode = data["Mesh"];
		if (!rootNode["MeshSource"] && !rootNode["MeshAsset"])
			return false;

		AssetHandle meshSource = rootNode["MeshSource"].as<uint64_t>();
		mesh = memory::Ref<grapichs::SkeletalMesh>::Create(meshSource);
		return true;
	}

	// SKELETON
	void AssetSerializer_Skeleton::Serialize(const AssetMetadata& metadata, const memory::Ref<Asset>& asset)
	{
		memory::Ref<grapichs::Skeleton> skeleton = asset.As<grapichs::Skeleton>();

		std::string yamlString = SerializeToFile(skeleton);
		std::filesystem::path serializePath = metadata.FilePath;
		std::ofstream fout(serializePath);

		if (!fout.is_open())
			return;

		fout << yamlString;
		fout.flush();
		fout.close();
	}

	bool AssetSerializer_Skeleton::TryLoadData(const AssetMetadata& metadata, memory::Ref<Asset>& asset)
	{
		std::ifstream stream(metadata.FilePath);
		std::stringstream strStream;
		strStream << stream.rdbuf();
		memory::Ref<grapichs::Skeleton> skeleton;

		bool serialized = TryLoadFromFile(strStream.str(), skeleton);
		if (!serialized)
			return false;

		skeleton->SetAssetHandle(metadata.Handle);
		asset = skeleton;
		return true;
	}

	std::string AssetSerializer_Skeleton::SerializeToFile(memory::Ref<grapichs::Skeleton>& skeleton) const {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Skeleton";
		{
			out << YAML::BeginMap;
			out << YAML::Key << "BoneCount";
			out << YAML::Value << skeleton->GetBoneCount();
			out << YAML::EndMap;
		}
		out << YAML::EndMap;
		return std::string(out.c_str());
	}

	bool AssetSerializer_Skeleton::TryLoadFromFile(const std::string& filePath, memory::Ref<grapichs::Skeleton>& skeleton) {
		YAML::Node data = YAML::Load(filePath);
		if (!data["Skeleton"])
			return false;

		YAML::Node rootNode = data["Skeleton"];
		if (!rootNode["BoneCount"])
			return false;

		uint32_t boneCount = rootNode["BoneCount"].as<uint32_t>();
		skeleton = memory::Ref<grapichs::Skeleton>::Create();
		return true;
	}

	// ANIMATION SERIALIZER
	void AssetSerializer_Animation::Serialize(const AssetMetadata& metadata, const memory::Ref<Asset>& asset) {
		memory::Ref<grapichs::Animation> animation = asset.As<grapichs::Animation>();

		std::string yamlString = SerializeToFile(animation);
		std::filesystem::path serializePath = metadata.FilePath;
		std::ofstream fout(serializePath);

		if (!fout.is_open()) return;
		fout << yamlString;
		fout.close();
	}

	bool AssetSerializer_Animation::TryLoadData(const AssetMetadata& metadata, memory::Ref<Asset>& asset) {
		if (metadata.FilePath.extension() == ".fbx" || metadata.FilePath.extension() == ".gltf" || metadata.FilePath.extension() == ".dae") {
			AnimationImporter importer(metadata.FilePath);
			memory::Ref<grapichs::Animation> animation = importer.Import();
			if (!animation) return false;

			animation->SetAssetHandle(metadata.Handle);
			asset = animation;
			return true;
		}

		std::ifstream stream(metadata.FilePath);
		std::stringstream strStream;
		strStream << stream.rdbuf();
		memory::Ref<grapichs::Animation> animation;

		bool serialized = TryLoadFromFile(strStream.str(), animation);
		if (!serialized) return false;

		animation->SetAssetHandle(metadata.Handle);
		asset = animation;
		return true;
	}

	std::string AssetSerializer_Animation::SerializeToFile(memory::Ref<grapichs::Animation>& animation) const {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Animation";
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Name";
			out << YAML::Value << animation->GetName();
			out << YAML::Key << "Duration";
			out << YAML::Value << animation->GetDuration();
			out << YAML::EndMap;
		}
		out << YAML::EndMap;
		return std::string(out.c_str());
	}

	bool AssetSerializer_Animation::TryLoadFromFile(const std::string& filePath, memory::Ref<grapichs::Animation>& animation) {
		return false;
	}
}