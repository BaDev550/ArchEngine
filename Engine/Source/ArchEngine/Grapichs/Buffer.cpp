#include "ArchPch.h"
#include "Buffer.h"

#include "ArchEngine/Core/Application.h"

namespace ae::grapichs {
	static const vk::DeviceSize GetAlignment(vk::DeviceSize instanceSize, vk::DeviceSize minOffsetAlignment) {
		if (minOffsetAlignment > 0)
			return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
		return instanceSize;
	}

	Buffer::Buffer(
		uint64_t size,
		vk::BufferUsageFlags usage,
		vk::MemoryPropertyFlags memoryProperties,
		bool mapAtConstructor
	) 
		: _context(Application::Get()->GetWindow().GetRenderContext()),
		_instanceSize(size),
		_alignmentSize(GetAlignment(size, 1)),
		_bufferSize(_alignmentSize * _instanceCount),
		_instanceCount(1),
		_usageFlags(usage),
		_memoryPropertyFlags(memoryProperties)
	{
		_context.CreateBuffer(_bufferSize, _usageFlags, _memoryPropertyFlags, _buffer, _bufferMemory);
		if (mapAtConstructor)
			Map(size);
	}

	Buffer::~Buffer() {
		Unmap();
		_context.GetDevice().destroyBuffer(_buffer);
		_context.GetDevice().freeMemory(_bufferMemory);
	}

	void Buffer::Map(uint64_t size, uint64_t offset){
		CHECKF(_buffer && _bufferMemory, "Called Map buffer before buffer was created!");
		CHECKF(_data = _context.GetDevice().mapMemory(_bufferMemory, offset, size), "failed to map memory");
	}

	void Buffer::Unmap(){
		if (_data) {
			_context.GetDevice().unmapMemory(_bufferMemory);
			_data = nullptr;
		}
	}

	void Buffer::Write(void* data, uint64_t size, uint64_t offset) {
		CHECKF(_data, "Cannot write to unmapped buffer");
		if (size == vk::WholeSize) {
			memcpy(_data, data, _bufferSize);
		}
		else {
			char* memOffset = (char*)_data;
			memOffset += offset;
			memcpy(memOffset, data, _bufferSize);
		}
	}

}