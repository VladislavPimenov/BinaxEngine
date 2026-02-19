#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform float time;
uniform vec3 fogColor;

void main() {
    vec3 color = texture(skybox, TexCoords).rgb;
    
    // Добавим легкую анимацию
    float brightness = sin(time * 0.5) * 0.1 + 0.9;
    color *= brightness;
    
    FragColor = vec4(color, 1.0);
}