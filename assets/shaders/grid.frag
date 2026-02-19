#version 330 core
out vec4 FragColor;

in vec3 WorldPos;

uniform vec3 viewPos;

void main() {
    float gridSize = 1.0;
    float width = 0.02;
    
    // Координаты в плоскости XZ
    vec2 pos = WorldPos.xz;
    
    // Расчет расстояния до линий сетки
    vec2 grid = abs(fract(pos / gridSize - 0.5) - 0.5) / fwidth(pos) * gridSize;
    float line = min(grid.x, grid.y);
    
    // Основные линии (каждая единица)
    float mainLine = line;
    
    // Центральные оси (X и Z) - красная и синяя
    float axisX = abs(WorldPos.x) < 0.03 ? 1.0 : 0.0;
    float axisZ = abs(WorldPos.z) < 0.03 ? 1.0 : 0.0;
    
    // Цвет сетки в зависимости от расстояния
    float dist = length(WorldPos.xz);
    float fade = max(0.0, 1.0 - dist * 0.1);
    
    vec3 gridColor = vec3(0.4, 0.4, 0.5) * fade;
    vec3 axisColorX = vec3(1.0, 0.3, 0.3); // красная ось X
    vec3 axisColorZ = vec3(0.3, 0.3, 1.0); // синяя ось Z
    
    // Смешиваем
    vec3 color = gridColor;
    if (mainLine < width) {
        color = mix(color, vec3(0.8, 0.8, 1.0), 0.5);
    }
    if (axisX > 0.0) {
        color = mix(color, axisColorX, 0.7);
    }
    if (axisZ > 0.0) {
        color = mix(color, axisColorZ, 0.7);
    }
    
    // Прозрачность вдалеке
    float alpha = mainLine < width ? fade * 0.8 : 0.0;
    alpha = max(alpha, max(axisX, axisZ) * fade);
    
    FragColor = vec4(color, alpha);
}