#version 460
layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec2 vTexCoords;
layout(set = 1, binding = 0) uniform sampler2D uAlbedoTexture;
layout(set = 1, binding = 1) uniform sampler2D uNormTexture;

void main() {
	vec4 textureColor = texture(uAlbedoTexture, vTexCoords);
	FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}