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

	StaticMesh::StaticMesh(AssetHandle meshSource) : _meshSource(meshSource) {
		if (auto meshSourceAsset = AssetManager::GetAsset<MeshSource>(meshSource); meshSourceAsset) {
			const std::vector<AssetHandle>& meshMaterials = meshSourceAsset->GetMaterials();
			for (size_t i = 0; i < meshMaterials.size(); i++) {
				_materials[(uint32_t)i] = AssetManager::GetAsset<MaterialAsset>(meshMaterials[i]);
			}
		}
	}
}