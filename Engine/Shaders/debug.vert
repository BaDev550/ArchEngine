#version 450
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;

layout(location = 0) out vec3 vFragColor;

layout(push_constant) uniform pcData {
    mat4 viewProjection;
} uPc;

void main() {
    gl_Position = uPc.viewProjection * vec4(aPosition, 1.0);
    vFragColor = aColor;
}