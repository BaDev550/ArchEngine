#version 460
layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 vTexCoords;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vWorldPos;

layout(set = 1, binding = 0) uniform sampler2D uAlbedoTexture;
layout(set = 1, binding = 1) uniform sampler2D uNormTexture;

layout(set = 0, binding = 0) uniform CameraData {
	mat4 View;
	mat4 Proj;
    vec3 Position;
} uCamera;

layout(set = 0, binding = 1) uniform samplerCube uSkyboxTexture;

void main() {
    vec4 albedoColor = texture(uAlbedoTexture, vTexCoords);
    vec3 viewDir = normalize(vWorldPos - uCamera.Position);
    vec3 normal = normalize(vNormal);
    vec3 reflectDir = reflect(viewDir, normal);
    vec4 envColor = texture(uSkyboxTexture, reflectDir);

    FragColor = mix(albedoColor, envColor, 0.3);
}