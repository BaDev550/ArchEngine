#pragma once
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <atomic>
#include <vector>
#include <mutex>
#include <type_traits>
#include <iostream>
#include "ArchEngine/Utilities/Logger.h"

#define AE_MEMORY_DEBUG_ALLOCATION true
#define AE_MEMORY_DEBUG_GARBAGECOLLECTOR false
#define AE_MEMORY_USE_GARBACE_COLLECTOR false
#define AE_MEMORY_USE_CUSTOM_ALLOCATOR false

namespace ae::memory {
	class GarbageCollector;
	struct AllocationMetrics {
		std::atomic<uint32_t> TotalAllocated = 0;
		std::atomic<uint32_t> TotalFreed = 0;

		uint32_t CurrentUsage() { return TotalAllocated - TotalFreed; }
	} static s_AllocationMetrics;

	static void* Allocate(size_t size) {
		s_AllocationMetrics.TotalAllocated += size;
		return malloc(size);
	}

	static void Free(void* memory, size_t size) {
		if (memory == nullptr) {
#if AE_MEMORY_DEBUG_ALLOCATION
			Logger_app::warn("Memory block is already destroyed.");
#endif
			return;
		}

		s_AllocationMetrics.TotalFreed += size;
		free(memory);
	}

	static void PrintMemoryUsage() {
#if AE_MEMORY_DEBUG_ALLOCATION
		Logger_app::trace("Current emory usage: {} bytes", s_AllocationMetrics.CurrentUsage());
#endif
	}

	class RefCounted {
	public:
		RefCounted() : _refCount(0) {}
		virtual ~RefCounted() = default;

		void IncRef() const {
			_refCount++;
		}

		void DecRef() const {
			_refCount--;
			if (_refCount == 0)
				delete this;
		}

		uint32_t GetRefCount() const { return _refCount; }
	private:
		mutable std::atomic<uint32_t> _refCount = 0;
	};

#if AE_MEMORY_USE_GARBACE_COLLECTOR
	class GarbageCollector {
	public:
		static void AddToQueue(const RefCounted* obj) {
			std::lock_guard<std::mutex> lock(s_mutex);
			s_deathQueue.push_back(const_cast<RefCounted*>(obj));
		}
		static void Collect() {
			std::lock_guard<std::mutex> lock(s_mutex);
			if (s_deathQueue.empty()) return;

#if AE_MEMORY_DEBUG_GARBAGECOLLECTOR
			Logger_app::trace("Garbage collector collecting: {} objects...", s_deathQueue.size());
#endif
			for (auto* obj : s_deathQueue)
				delete obj;
			s_deathQueue.clear();
		}
	private:
		inline static std::vector<RefCounted*> s_deathQueue;
		inline static std::mutex s_mutex;
	};
#endif

	template<typename T>
	class Ref {
	public:
		Ref() : _instance(nullptr) {}
		Ref(std::nullptr_t) : _instance(nullptr) {}
		Ref(T* instance) : _instance(instance) {
			IncRef();
		}
		Ref(const Ref<T>& other) : _instance(other._instance) {
			IncRef();
		}
		Ref(Ref<T>&& other) noexcept : _instance(other._instance) {
			other._instance = nullptr;
		}
		~Ref() { DecRef(); }
		Ref& operator=(const Ref<T>& other) {
			if (this == &other) return *this;
			DecRef();
			_instance = other._instance;
			IncRef();
			return *this;
		}

		T* operator->() { return _instance; }
		const T* operator->() const { return _instance; }
		T& operator*() { return *_instance; }
		const T& operator*() const { return *_instance; }

		operator bool() const { return _instance != nullptr; }
		
		T* Get() { return _instance; }
		bool operator==(const Ref<T>& other) const { return _instance == other._instance; }
		bool operator!=(const Ref<T>& other) const { return _instance != other._instance; }

		template<typename... Args>
		static Ref<T> Create(Args&&... args) {
			return Ref<T>(new T(std::forward<Args>(args)...));
		}
	private:
		void IncRef() const {
			static_assert(std::is_base_of<RefCounted, T>::value, "instance is not ref counted");
			if (_instance) _instance->IncRef();
		}
		void DecRef() const {
			static_assert(std::is_base_of<RefCounted, T>::value, "instance is not ref counted");
			if (_instance) {
				_instance->DecRef();
			}
		}
		mutable T* _instance;
	};

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T, typename... Args>
	Scope<T> MakeScope(Args&&... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}
}

#if AE_MEMORY_USE_CUSTOM_ALLOCATOR
void* operator new(size_t size) {
	return ae::memory::Allocate(size);
}

void* operator new[](size_t size) {
	return ae::memory::Allocate(size);
}

void operator delete(void* memory, size_t size) noexcept {
	return ae::memory::Free(memory, size);
}
#endif