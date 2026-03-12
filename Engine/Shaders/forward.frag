#version 460
layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 vTexCoords;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vWorldPos;

#include "common/buffers.glslh"
#include "common/resources.glslh"

void main() {
    vec4 albedoColor = texture(uAlbedoTexture, vTexCoords);
    vec3 viewDir = normalize(vWorldPos - uCamera.Position);
    vec3 normal = normalize(vNormal);
    vec3 reflectDir = reflect(viewDir, normal);
    vec4 envColor = texture(uSkyboxTexture, reflectDir);

    FragColor = albedoColor;
}