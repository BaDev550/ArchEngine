#version 460
layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec3 vLocalPos;

layout(set = 0, binding = 1) uniform samplerCube uSkyboxTexture;

void main() {
    FragColor = texture(uSkyboxTexture, vLocalPos);
}