#version 460

layout(push_constant) uniform pcData {
    mat4 Transform;
} pc;

vec3 positions[3] = vec3[](
    vec3( 0.0, -0.5, 0.0), // Top
    vec3(-0.5,  0.5, 0.0), // Bottom Left
    vec3( 0.5,  0.5, 0.0)  // Bottom Right
);

void main() {
	gl_Position = vec4(positions[gl_VertexIndex], 1.0);
} 