#version 460
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;
layout(location = 5) in ivec4 aBoneIDs;
layout(location = 6) in vec4  aBoneWeights;

layout(location = 0) out vec2 vTexCoords;
layout(location = 1) out vec3 vNormal;
layout(location = 2) out vec3 vWorldPos;

layout(push_constant) uniform pcData {
    mat4 Transform;
} uPc;

#include "common/buffers.glslh"

void main() {
vec4 totalLocalPos = vec4(0.0);
    vec3 totalNormal = vec3(0.0);

    if (aBoneIDs[0] == -1) {
        totalLocalPos = vec4(aPosition, 1.0);
        totalNormal = aNormal;
    } else {
        for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++) {
            if(aBoneIDs[i] == -1) 
                continue;
              
            if(aBoneIDs[i] >= MAX_BONES) {
                totalLocalPos = vec4(aPosition, 1.0);
                totalNormal = aNormal;
                break;
            }

            mat4 boneMat = uBones.Bones[aBoneIDs[i]];
            float weight = aBoneWeights[i];

            vec4 localPosition = boneMat * vec4(aPosition, 1.0);
            totalLocalPos += localPosition * weight;

            vec3 localNormal = mat3(boneMat) * aNormal; 
            totalNormal += localNormal * weight;
        }
    }

    vec4 worldPos = uPc.Transform * totalLocalPos;
    
    vTexCoords = aTexCoords;
    vNormal = totalNormal; 
    vWorldPos = worldPos.xyz;
    gl_Position = uCamera.Proj * uCamera.View * worldPos;
} 