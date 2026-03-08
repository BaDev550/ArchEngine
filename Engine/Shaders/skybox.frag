#version 460
layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec3 vTexCoords;
layout(set = 0, binding = 1) uniform samplerCube uSkyboxTexture; 

void main() {
    FragColor = texture(uSkyboxTexture, vTexCoords);
}