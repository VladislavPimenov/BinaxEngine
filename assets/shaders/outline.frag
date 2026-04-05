#version 330 core
uniform vec3 outlineColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(outlineColor, 1.0);
}