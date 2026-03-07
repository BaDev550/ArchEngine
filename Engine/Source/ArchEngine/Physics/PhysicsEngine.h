#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>

namespace ae::physics {
	namespace Layers
	{
		static constexpr JPH::ObjectLayer NON_MOVING = 0;
		static constexpr JPH::ObjectLayer MOVING = 1;
		static constexpr uint32_t NUM_LAYERS = 2;
	}
	namespace BroadPhaseLayers
	{
		static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
		static constexpr JPH::BroadPhaseLayer MOVING(1);
		static constexpr uint32_t NUM_LAYERS = 2;
	}

	class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface {
	public:
		BPLayerInterfaceImpl() {
			mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
			mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
		}
		uint32_t GetNumBroadPhaseLayers() const override {
			return BroadPhaseLayers::NUM_LAYERS;
		}

		virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override {
			if (inLayer == BroadPhaseLayers::MOVING) return "MOVING";
			if (inLayer == BroadPhaseLayers::NON_MOVING) return "NON_MOVING";
			return "";
		}
		JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer layer) const override {
			return mObjectToBroadPhase[layer];
		}
	private:
		JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
	};

	class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter {
	public:
		bool ShouldCollide(JPH::ObjectLayer layer1, JPH::BroadPhaseLayer layer2) const override {
			switch (layer1)
			{
			case Layers::NON_MOVING:
				return layer2 == BroadPhaseLayers::MOVING;
			case Layers::MOVING:
				return true;
			}
			return false;
		}
	};

	class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter {
	public:
		bool ShouldCollide(JPH::ObjectLayer layer1, JPH::ObjectLayer layer2) const override {
			if (layer1 == Layers::NON_MOVING && layer2 == Layers::NON_MOVING)
				return false;

			return true;
		}
	};

	struct PhysicsSettings {
		uint32_t MaxBodies = 10240;
		uint32_t NumBodyMutexes = 0;
		uint32_t MaxBodyPairs = 65536;
		uint32_t MaxContactConstraints = 10240;
		uint32_t TempAllocatorSize = 10 * 1024 * 1024;
		uint32_t MaxPhysicsJobs = 1024;
		uint32_t MaxPhysicsBarriers = 1024;
		uint32_t CollisionSteps = 10;
	};

	class PhysicsEngine {
	public:
		static void Init(const PhysicsSettings& settings);
		static void Shutdown();
		static void Update(float deltaTime);

		static JPH::PhysicsSystem& GetSystem() { return *_system; }
		static JPH::BodyInterface& GetBodyInterface() { return _system->GetBodyInterface(); }
	private:
		static PhysicsSettings _settings;
		static JPH::PhysicsSystem* _system;
		static JPH::TempAllocator* _allocator;
		static JPH::JobSystem* _jobSystem;
		static BPLayerInterfaceImpl _broadPhaseLayerInterface;
		static ObjectVsBroadPhaseLayerFilterImpl _objectVsBroadPhaseLayerFilter;
		static ObjectLayerPairFilterImpl _objectLayerPairFilter;
	};
}