#version 460
layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 vTexCoords;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vWorldPos;

#include "common/buffers.glslh"
#include "common/resources.glslh"

const int CASCADE_COUNT = 4;

float ShadowCalculation()
{
    vec4 fragPosViewSpace = uCamera.View * vec4(vWorldPos, 1.0);
    float depthValue = abs(fragPosViewSpace.z);
    
    int layer = -1;
    for (int i = 0; i < CASCADE_COUNT - 1; i++) {
        if (depthValue < uCascadeShadow.CascadeSplits[i]){
            layer = i;
            break;
        }
    }
    if (layer == -1)
        layer = CASCADE_COUNT - 1;

    vec4 fragPosLightSpace = uCascadeShadow.LightSpaceMatrices[layer] * vec4(vWorldPos, 1.0);

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;
    if (currentDepth > 1.0f)
        return 0.0f;

    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(uDirectionalLight.Light.Direction);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    const float biasModifier = 0.5f;
    if (layer == CASCADE_COUNT - 1){
        bias *= 1.0f / (uCascadeShadow.CascadeSplits[layer - 1] * biasModifier);
    }else {
        bias *= 1.0f / (uCascadeShadow.CascadeSplits[layer] * biasModifier);
    }

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(uShadowMapTexture, 0).xy;
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(uShadowMapTexture, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
        
    return shadow;
}

void main() {
    vec4 albedoColor = texture(uAlbedoTexture, vTexCoords);
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(uDirectionalLight.Light.Direction);
    vec4 viewPos = uCamera.View * vec4(vWorldPos, 1.0);
    float depthValue = abs(viewPos.z);

    float shadow = ShadowCalculation();

    float ambient = 0.02;
    vec3 finalLighting = (ambient + (1.0 - shadow)) * albedoColor.rgb;

    FragColor = vec4(finalLighting, albedoColor.a);
}