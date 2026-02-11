#pragma once
#include "RenderContext.h"
#include "ArchEngine/Core/Memory.h"
#include "ArchEngine/Utilities/Defines.h"

namespace ae::grapichs {
	class Buffer : public memory::RefCounted {
	public:
		Buffer(
			uint64_t size,
			vk::BufferUsageFlags usage,
			vk::MemoryPropertyFlags memoryProperties,
			bool mapAtConstructor = false
		);
		~Buffer();

		void Map(uint64_t size = UINT64_MAX, uint64_t offset = 0);
		void Unmap();
		void Write(void* data, uint64_t size = UINT64_MAX, uint64_t offset = 0);

		vk::DescriptorBufferInfo GetDescriptorInfo(vk::DeviceSize size = vk::WholeSize, uint64_t offset = 0) const { return { .buffer = _buffer, .offset = offset, .range = size }; }
		vk::Buffer GetBuffer() const { return _buffer; }
		void* GetData() const { return _data; }
	private:
		void* _data = nullptr;
		uint32_t _instanceCount;
		vk::Buffer _buffer;
		vk::DeviceMemory _bufferMemory;
		vk::DeviceSize _bufferSize;
		vk::DeviceSize _instanceSize;
		vk::DeviceSize _alignmentSize;
		vk::BufferUsageFlags _usageFlags;
		vk::MemoryPropertyFlags _memoryPropertyFlags;

		RenderContext& _context;
	};
}