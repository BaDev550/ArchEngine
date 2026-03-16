#version 460
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;
layout(location = 5) in ivec4 aBoneIDs;
layout(location = 6) in vec4  aBoneWeights;

layout(push_constant) uniform pcData {
    mat4 Transform;
	mat4 LightSpaceMatrix;
} uPc;

void main() {
	gl_Position = uPc.LightSpaceMatrix * uPc.Transform * vec4(aPosition, 1.0);
} 