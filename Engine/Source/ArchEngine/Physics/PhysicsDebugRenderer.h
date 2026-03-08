#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include "ArchEngine/Grapichs/DebugRenderer.h"
#include "ArchEngine/Utilities/Math.h"

namespace ae::physics::debug {
    class JoltBatchImpl : public JPH::RefTargetVirtual {
    public:
        JoltBatchImpl(const JPH::DebugRenderer::Triangle* inTriangles, int inTriangleCount) {}
        JoltBatchImpl(const JPH::DebugRenderer::Vertex* inVertices, int inVertexCount, const JPH::uint32* inIndices, int inIndexCount) {}

        virtual void AddRef() override { ++mRefCount; }
        virtual void Release() override { if (--mRefCount == 0) delete this; }
    private:
        std::atomic<uint32_t> mRefCount = 0;
    };

	class PhysicsDebugRenderer : public JPH::DebugRenderer {
	public:
        PhysicsDebugRenderer() {
            Initialize();
        }
        virtual ~PhysicsDebugRenderer() override = default;

        virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override {
            ae::grapichs::debug::DebugRenderer::DrawLine(math::JoltToGLM(inFrom), math::JoltToGLM(inTo), math::JoltToGLM(inColor));
        }

        virtual void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow) override {
            ae::grapichs::debug::DebugRenderer::DrawTriangle(math::JoltToGLM(inV1), math::JoltToGLM(inV2), math::JoltToGLM(inV3), math::JoltToGLM(inColor));
        }

        virtual void DrawGeometry(JPH::RMat44Arg inModelMatrix, const JPH::AABox& inWorldSpaceBounds, float inLODScaleSq, JPH::ColorArg inModelColor, const GeometryRef& inGeometry, ECullMode inCullMode, ECastShadow inCastShadow, EDrawMode inDrawMode) override {}
        virtual void DrawText3D(JPH::RVec3Arg inPosition, const std::string_view& inString, JPH::ColorArg inColor, float inHeight) override {}

        virtual Batch CreateTriangleBatch(const Triangle* inTriangles, int inTriangleCount) override {
            return new JoltBatchImpl(inTriangles, inTriangleCount);
        }

        virtual Batch CreateTriangleBatch(const Vertex* inVertices, int inVertexCount, const JPH::uint32* inIndices, int inIndexCount) override {
            return new JoltBatchImpl(inVertices, inVertexCount, inIndices, inIndexCount);
        }

	};
}