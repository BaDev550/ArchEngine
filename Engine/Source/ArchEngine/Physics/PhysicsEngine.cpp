#include "ArchPch.h"
#include "PhysicsEngine.h"

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "ArchEngine/Grapichs/DebugRenderer.h"

namespace ae::physics {
	PhysicsSettings PhysicsEngine::_settings;
	JPH::PhysicsSystem* PhysicsEngine::_system = nullptr;
	JPH::TempAllocator* PhysicsEngine::_allocator = nullptr;
	JPH::JobSystem* PhysicsEngine::_jobSystem = nullptr;
    debug::PhysicsDebugRenderer* PhysicsEngine::_debugRenderer = nullptr;
    BPLayerInterfaceImpl PhysicsEngine::_broadPhaseLayerInterface;
    ObjectVsBroadPhaseLayerFilterImpl PhysicsEngine::_objectVsBroadPhaseLayerFilter;
    ObjectLayerPairFilterImpl PhysicsEngine::_objectLayerPairFilter;

	void PhysicsEngine::Init(const PhysicsSettings& settings) {
        _settings = settings;
        JPH::RegisterDefaultAllocator();

        JPH::Factory::sInstance = new JPH::Factory();
        JPH::RegisterTypes();

        _allocator = new JPH::TempAllocatorMalloc();
        _debugRenderer = new debug::PhysicsDebugRenderer();
        _jobSystem = new JPH::JobSystemThreadPool(
            _settings.MaxPhysicsJobs,
            _settings.MaxPhysicsBarriers,
            std::thread::hardware_concurrency() - 1
        );

        _system = new JPH::PhysicsSystem();
        _system->Init(
            _settings.MaxBodies,
            _settings.NumBodyMutexes,
            _settings.MaxBodyPairs,
            _settings.MaxContactConstraints,
            _broadPhaseLayerInterface,
            _objectVsBroadPhaseLayerFilter,
            _objectLayerPairFilter
        );
        _system->SetGravity({ 0.0f, -9.81, 0.0f });
	}

    void PhysicsEngine::Update(float deltaTime) {
        _system->Update(deltaTime, _settings.CollisionSteps, _allocator, _jobSystem);

        JPH::BodyManager::DrawSettings drawSettings;
        drawSettings.mDrawBoundingBox = true;
        drawSettings.mDrawShape = true;
        drawSettings.mDrawShapeWireframe = true;
        _system->DrawBodies(drawSettings, _debugRenderer);
    }

	void PhysicsEngine::Shutdown() {
        delete _system;
        _system = nullptr;

        delete _jobSystem;
        _jobSystem = nullptr;

        delete _debugRenderer;
        _debugRenderer = nullptr;

        delete _allocator;
        _allocator = nullptr;

        JPH::UnregisterTypes();

        delete JPH::Factory::sInstance;
        JPH::Factory::sInstance = nullptr;
	}
}