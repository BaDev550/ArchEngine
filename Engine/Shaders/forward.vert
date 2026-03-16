#version 460
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;
layout(location = 5) in ivec4 aBoneIDs;
layout(location = 6) in vec4 aBoneWeights;

layout(location = 0) out vec2 vTexCoords;
layout(location = 1) out vec3 vNormal;
layout(location = 2) out vec3 vWorldPos;
#define MAX_BONE_INFLUENCE 4
#define MAX_BONES 256

layout(push_constant) uniform pcData {
    mat4 Transform;
	int IsSkinned;
} uPc;

layout(set = 2, binding = 0) uniform BoneUniform {
	mat4 BoneMatrices[MAX_BONES];
} uBones;

#include "common/buffers.glslh"

void main() {
    mat4 finalTransform = uPc.Transform;
    vec4 localPos = vec4(aPosition, 1.0);
    vec3 localNormal = aNormal;

    if (uPc.IsSkinned == 1) {
        mat4 skinMat = 
            aBoneWeights.x * uBones.BoneMatrices[aBoneIDs.x] +
            aBoneWeights.y * uBones.BoneMatrices[aBoneIDs.y] +
            aBoneWeights.z * uBones.BoneMatrices[aBoneIDs.z] +
            aBoneWeights.w * uBones.BoneMatrices[aBoneIDs.w];
        localPos = skinMat * localPos;
        localNormal = mat3(skinMat) * aNormal;
    }

    vec4 worldPos = finalTransform * localPos;
    vTexCoords = aTexCoords;
    vNormal = normalize((finalTransform * vec4(localNormal, 0.0)).xyz);
    vWorldPos = worldPos.xyz;
    gl_Position = uCamera.Proj * uCamera.View * worldPos;
} 