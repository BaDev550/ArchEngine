#version 460

layout(set = 0, binding = 1) uniform MyUniformBuffer {
    vec3 AUniform;
} uniformA;
layout(push_constant) uniform pcData {
    mat4 Transform;
} pc;
layout(set = 0, binding = 0) uniform sampler2D BSampler;

void main() {
	gl_Position = vec4(0.0f, 0.0f, 0.0f, 1.0f);
} 