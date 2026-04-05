#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in mat3 TBN;
in vec4 FragPosLightSpace;

uniform sampler2D diffuseTexture;
uniform sampler2D normalMap;
uniform sampler2D roughnessTexture;
uniform sampler2D metallicTexture;
uniform sampler2D aoTexture;
uniform bool hasDiffuseTexture;
uniform bool hasNormalMap;
uniform bool hasRoughnessTexture;
uniform bool hasMetallicTexture;
uniform bool hasAOTexture;
uniform vec3 objectColor;

uniform float metallic;
uniform float roughness;
uniform float normalStrength;
uniform vec2 uvScale;
uniform bool useWorldUV;

uniform vec3 viewPos;

// === ТЕНИ ===
uniform sampler2D shadowMap;
uniform bool shadowsEnabled;
uniform bool receiveShadows;
uniform float shadowBias;
uniform float shadowSoftness;
uniform int shadowSamples;

// === ЭМИССИЯ ===
uniform vec3 emissionColor;
uniform float emissionIntensity;

// === AMBIENT ===
uniform float ambientStrength;

// === СТРУКТУРА СВЕТА ===
struct Light {
    int type;          // 0 = directional, 1 = point, 2 = spot
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float range;
    float angle;
};
uniform Light lights[8];
uniform int numLights;

float ShadowCalculation(vec4 fragPosLightSpace, float NdotL) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    if (projCoords.z > 1.0) return 0.0;

    float currentDepth = projCoords.z;
    float bias = max(shadowBias, 0.05 * (1.0 - NdotL));
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    if (shadowSamples == 4) {
        for (int x = -1; x <= 1; x += 2) {
            for (int y = -1; y <= 1; y += 2) {
                vec2 offset = vec2(x, y) * texelSize * shadowSoftness;
                float pcfDepth = texture(shadowMap, projCoords.xy + offset).r;
                shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;
            }
        }
        shadow /= 4.0;
    } else {
        for (int x = -1; x <= 1; x++) {
            for (int y = -1; y <= 1; y++) {
                vec2 offset = vec2(x, y) * texelSize * shadowSoftness;
                float pcfDepth = texture(shadowMap, projCoords.xy + offset).r;
                shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;
            }
        }
        shadow /= 9.0;
    }
    return shadow;
}

void main() {
    // UV и альбедо
    vec2 uv = TexCoords * uvScale;
    vec3 albedo;
    if (useWorldUV) {
        vec3 worldPos = FragPos;
        vec3 blend = abs(normalize(TBN[2]));
        blend = pow(blend, vec3(2.0));
        blend /= blend.x + blend.y + blend.z;
        vec3 xaxis = texture(diffuseTexture, worldPos.yz * uvScale).rgb;
        vec3 yaxis = texture(diffuseTexture, worldPos.xz * uvScale).rgb;
        vec3 zaxis = texture(diffuseTexture, worldPos.xy * uvScale).rgb;
        albedo = xaxis * blend.x + yaxis * blend.y + zaxis * blend.z;
    } else {
        if (hasDiffuseTexture)
            albedo = texture(diffuseTexture, uv).rgb;
        else
            albedo = objectColor;
    }

    // Нормаль
    vec3 geomNormal = normalize(TBN[2]);
    vec3 normal;
    if (hasNormalMap && !useWorldUV) {
        vec3 tangentNormal = texture(normalMap, uv).rgb * 2.0 - 1.0;
        vec3 worldNormal = normalize(TBN * tangentNormal);
        normal = normalize(mix(geomNormal, worldNormal, normalStrength));
    } else {
        normal = geomNormal;
    }

    // PBR параметры из текстур или uniform
    float metallicVal = metallic;
    if (hasMetallicTexture) metallicVal = texture(metallicTexture, uv).r;
    float roughnessVal = roughness;
    if (hasRoughnessTexture) roughnessVal = texture(roughnessTexture, uv).r;
    float aoVal = 1.0;
    if (hasAOTexture) aoVal = texture(aoTexture, uv).r;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = ambientStrength * albedo * aoVal;

    // Перебор всех источников света
    for (int i = 0; i < numLights; i++) {
        Light light = lights[i];
        vec3 lightDir;
        float attenuation = 1.0;
        float NdotL = 0.0;

        if (light.type == 0) { // directional
            lightDir = normalize(-light.direction);
            NdotL = max(dot(normal, lightDir), 0.0);
            attenuation = 1.0;
        } else if (light.type == 1) { // point
            vec3 delta = light.position - FragPos;
            float dist = length(delta);
            if (dist > light.range) continue;
            lightDir = delta / dist;
            NdotL = max(dot(normal, lightDir), 0.0);
            attenuation = 1.0 / (1.0 + dist * dist / (light.range * light.range));
        } else if (light.type == 2) { // spot
            vec3 delta = light.position - FragPos;
            float dist = length(delta);
            if (dist > light.range) continue;
            lightDir = delta / dist;
            NdotL = max(dot(normal, lightDir), 0.0);
            attenuation = 1.0 / (1.0 + dist * dist / (light.range * light.range));
            vec3 spotDir = normalize(light.direction);
            float cosTheta = dot(-lightDir, spotDir);
            float spotEffect = smoothstep(cos(light.angle), cos(light.angle * 0.5), cosTheta);
            attenuation *= spotEffect;
        }

        if (NdotL <= 0.0) continue;

        vec3 halfwayDir = normalize(lightDir + viewDir);
        float NdotH = max(dot(normal, halfwayDir), 0.0);
        float shininess = 2.0 / (roughnessVal * roughnessVal) - 2.0;
        shininess = clamp(shininess, 1.0, 512.0);
        float spec = pow(NdotH, shininess);
        vec3 specColor = mix(vec3(1.0), albedo, metallicVal);

        vec3 diffuse = NdotL * albedo * light.color * light.intensity;
        vec3 specular = spec * specColor * light.color * light.intensity;

        // Френель
        float fresnel = pow(1.0 - max(dot(viewDir, normal), 0.0), 2.0);
        specular += fresnel * 0.04 * (1.0 - metallicVal) * light.intensity;

        // Тени только для directional
        float shadow = 0.0;
        if (shadowsEnabled && receiveShadows && light.type == 0 && NdotL > 0.0) {
            shadow = ShadowCalculation(FragPosLightSpace, NdotL);
        }

        result += (1.0 - shadow) * attenuation * (diffuse + specular);
    }

    result += emissionColor * emissionIntensity;
    FragColor = vec4(result, 1.0);
}
