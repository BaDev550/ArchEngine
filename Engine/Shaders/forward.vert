#version 460
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

layout(push_constant) uniform pcData {
    mat4 Transform;
} uPc;

layout(set = 0, binding = 0) uniform CameraData {
	mat4 View;
	mat4 Proj;
} uCamera;

void main() {
	gl_Position = uCamera.Proj * uCamera.View * uPc.Transform * vec4(aPosition, 1.0);
} 