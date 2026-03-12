#version 460
layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec3 vLocalPos;

#include "common/resources.glslh"

void main() {
    FragColor = texture(uSkyboxTexture, vLocalPos);
}