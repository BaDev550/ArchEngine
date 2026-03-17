#include <ArchPch.h>
#include "Model.h"
#include "Renderer.h"

#include "ArchEngine/AssetManager/AssetManager.h"

namespace ae::grapichs {
	void MeshSource::CreateVertexBuffer() {
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

	void MeshSource::CreateIndexBuffer() {
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

#if 0
	void MeshSource::AddBone(std::string name, Bone bone)
	{
		_bones[name] = bone;
		_boneCount++;
	}
	
	void MeshSource::AddBoneToVertex(Vertex& vertex, int boneID, float weight)
	{
		for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
			if (vertex.BoneIDs[i] < 0) {
				vertex.BoneIDs[i] = boneID;
				vertex.BoneWeights[i] = weight;
				break;
			}
		}
	}

	bool MeshSource::HasBone(std::string name) const
	{
		return _bones.find(name) != _bones.end();
	}
#endif

	StaticMesh::StaticMesh(AssetHandle meshSource) : _meshSource(meshSource) {
		if (auto meshSourceAsset = AssetManager::GetAsset<MeshSource>(meshSource); meshSourceAsset) {
			const std::vector<AssetHandle>& meshMaterials = meshSourceAsset->GetMaterials();
			for (size_t i = 0; i < meshMaterials.size(); i++) {
				_materials[(uint32_t)i] = AssetManager::GetAsset<MaterialAsset>(meshMaterials[i]);
			}
		}
	}

	SkeletalMesh::SkeletalMesh(AssetHandle meshSource)
		: _meshSource(meshSource)
	{
		if (auto meshSourceAsset = AssetManager::GetAsset<MeshSource>(meshSource); meshSourceAsset) {
			const std::vector<AssetHandle>& meshMaterials = meshSourceAsset->GetMaterials();
			for (size_t i = 0; i < meshMaterials.size(); i++) {
				_materials[(uint32_t)i] = AssetManager::GetAsset<MaterialAsset>(meshMaterials[i]);
			}
			auto& skeleton = meshSourceAsset->GetSkeleton();
			_bones = skeleton->GetBones();
			_bonesBuffer = memory::Ref<Buffer>::Create(
				sizeof(glm::mat4) * MAX_BONES, 
				vk::BufferUsageFlagBits::eUniformBuffer,
				vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
				true
			);
		}
	}
}