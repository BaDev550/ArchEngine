#version 460
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

layout(push_constant) uniform pcData {
    mat4 Transform;
} uPc;

#include "common/buffers.glslh"

void main() {
	gl_Position = uCamera.LightSpaceMatrix * uPc.Transform * vec4(aPosition, 1.0);
} 