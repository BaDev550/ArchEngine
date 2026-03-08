#version 460
layout(location = 0) in vec3 aPos;

layout(set = 0, binding = 0) uniform CameraData {
    mat4 view;
    mat4 projection;
} uCamera;

layout(location = 0) out vec3 vTexCoords;

void main() {
    vTexCoords = aPos;
    mat4 rotView = mat4(mat3(uCamera.view));
    vec4 clipPos = uCamera.projection * rotView * vec4(aPos, 1.0);
    gl_Position = clipPos.xyww; 
}