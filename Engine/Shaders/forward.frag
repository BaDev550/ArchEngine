#version 460
layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 vTexCoords;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vWorldPos;

#include "common/buffers.glslh"
#include "common/resources.glslh"

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(uShadowMapTexture, projCoords.xy).r; 
    float currentDepth = projCoords.z;

    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(-uDirectionalLight.Light.Direction);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(uShadowMapTexture, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(uShadowMapTexture, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main() {
    vec4 albedoColor = texture(uAlbedoTexture, vTexCoords);
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(uDirectionalLight.Light.Direction);

    vec4 fragPosLightSpace = uCamera.LightSpaceMatrix * vec4(vWorldPos, 1.0);
    float shadow = ShadowCalculation(fragPosLightSpace);

    float ambient = 0.02;
    vec3 finalLighting = (ambient + (1.0 - shadow)) * albedoColor.rgb;

    FragColor = vec4(finalLighting, albedoColor.a);
}