#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec4 FragPosLightSpace;

// Свойства материала
uniform vec3 objectColor;
uniform float shininess;
uniform float metallic;
uniform float roughness;

// Освещение
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float lightIntensity;
uniform vec3 viewPos;

// Для теней (пока заглушка)
uniform sampler2D shadowMap;

// Функция для расчета теней
float ShadowCalculation(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if(projCoords.z > 1.0)
        return 0.0;
    
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = 0.005;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    
    return shadow;
}

void main() {
    // Нормализуем нормали
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    
    // Параметры материала
    float metalness = metallic; // 0 - диэлектрик, 1 - металл
    float rough = roughness; // 0 - гладкий, 1 - шершавый
    float spec = shininess; // сила бликов
    
    // === ОСНОВНОЕ ОСВЕЩЕНИЕ (ФОНГ) ===
    
    // Ambient (очень слабый)
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor * objectColor;
    
    // Diffuse (основной цвет)
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * objectColor;
    
    // Specular (блики)
    float specularStrength = 0.5;
    vec3 reflectDir = reflect(-lightDir, norm);
    float specAngle = max(dot(viewDir, reflectDir), 0.0);
    float specular = pow(specAngle, spec * 4.0) * specularStrength;
    vec3 specularColor = specular * lightColor * mix(vec3(1.0), objectColor, metalness);
    
    // === ДОПОЛНИТЕЛЬНЫЙ СВЕТ (чтобы не было черноты) ===
    
    // Второй источник света (задний)
    vec3 backLightDir = normalize(vec3(-lightPos.x, lightPos.y, -lightPos.z) - FragPos);
    float backDiff = max(dot(norm, backLightDir), 0.0) * 0.3;
    vec3 backLight = backDiff * vec3(0.5, 0.5, 0.8); // голубоватый
    
    // Контурный свет (rim lighting)
    float rim = 1.0 - max(dot(viewDir, norm), 0.0);
    rim = pow(rim, 3.0) * 0.5;
    vec3 rimColor = rim * vec3(0.8, 0.6, 1.0);
    
    // === СВЕЧЕНИЕ (emissive) для ярких объектов ===
    vec3 emissive = vec3(0.0);
    if (length(objectColor - vec3(1.0, 0.9, 0.2)) < 0.3) {
        emissive = objectColor * 0.3; // желтые светятся
    }
    
    // === СБОРКА ===
    vec3 result = ambient + diffuse + specularColor + backLight + rimColor + emissive;
    
    // Применяем тени (пока отключим, т.к. нет карты теней)
    // float shadow = ShadowCalculation(FragPosLightSpace);
    // result = (ambient + (1.0 - shadow) * (diffuse + specularColor)) * objectColor + backLight + rimColor;
    
    // Немного коррекции цвета
    result = pow(result, vec3(1.0/2.2)); // гамма-коррекция
    
    FragColor = vec4(result, 1.0);
}