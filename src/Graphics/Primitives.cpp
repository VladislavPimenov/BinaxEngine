#include "Graphics/Primitives.h"
#include <vector>
#include <cmath>
#include <glm/glm.hpp>

std::shared_ptr<Mesh> Primitives::CreateCube() {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float cubeVertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,

        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f
    };

    unsigned int cubeIndices[] = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        8, 9, 10, 8, 10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23
    };

    for (int i = 0; i < 24 * 8; i += 8) {
        Vertex vertex;
        vertex.Position[0] = cubeVertices[i];
        vertex.Position[1] = cubeVertices[i + 1];
        vertex.Position[2] = cubeVertices[i + 2];
        vertex.Normal[0] = cubeVertices[i + 3];
        vertex.Normal[1] = cubeVertices[i + 4];
        vertex.Normal[2] = cubeVertices[i + 5];
        vertex.TexCoords[0] = cubeVertices[i + 6];
        vertex.TexCoords[1] = cubeVertices[i + 7];
        vertices.push_back(vertex);
    }

    for (int i = 0; i < 36; i++) {
        indices.push_back(cubeIndices[i]);
    }

    return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> Primitives::CreateGrid(int size) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float halfSize = size / 2.0f;

    float gridVertices[] = {
        -halfSize, 0.0f, -halfSize, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
         halfSize, 0.0f, -halfSize, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
         halfSize, 0.0f,  halfSize, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -halfSize, 0.0f,  halfSize, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };

    unsigned int gridIndices[] = { 0, 1, 2, 0, 2, 3 };

    for (int i = 0; i < 4 * 8; i += 8) {
        Vertex vertex;
        vertex.Position[0] = gridVertices[i];
        vertex.Position[1] = gridVertices[i + 1];
        vertex.Position[2] = gridVertices[i + 2];
        vertex.Normal[0] = gridVertices[i + 3];
        vertex.Normal[1] = gridVertices[i + 4];
        vertex.Normal[2] = gridVertices[i + 5];
        vertex.TexCoords[0] = gridVertices[i + 6];
        vertex.TexCoords[1] = gridVertices[i + 7];
        vertices.push_back(vertex);
    }

    for (int i = 0; i < 6; i++) {
        indices.push_back(gridIndices[i]);
    }

    return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> Primitives::CreateSphere(int segments) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    const float PI = 3.14159265359f;
    float radius = 0.5f;

    for (int y = 0; y <= segments; ++y) {
        for (int x = 0; x <= segments; ++x) {
            float xSegment = (float)x / (float)segments;
            float ySegment = (float)y / (float)segments;

            float xPos = radius * std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = radius * std::cos(ySegment * PI);
            float zPos = radius * std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

            Vertex vertex;
            vertex.Position[0] = xPos;
            vertex.Position[1] = yPos;
            vertex.Position[2] = zPos;

            // Нормаль равна нормализованной позиции (сфера центрирована в 0)
            vertex.Normal[0] = xPos / radius;
            vertex.Normal[1] = yPos / radius;
            vertex.Normal[2] = zPos / radius;

            vertex.TexCoords[0] = xSegment;
            vertex.TexCoords[1] = ySegment;

            vertices.push_back(vertex);
        }
    }

    for (int y = 0; y < segments; ++y) {
        for (int x = 0; x < segments; ++x) {
            int first = (y * (segments + 1)) + x;
            int second = first + segments + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> Primitives::CreateCylinder(int segments) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    const float PI = 3.14159265359f;
    float radius = 0.5f;
    float height = 1.0f;
    float halfHeight = height * 0.5f;

    // Боковая поверхность
    for (int i = 0; i <= segments; ++i) {
        float theta = (float)i / (float)segments * 2.0f * PI;
        float x = radius * cos(theta);
        float z = radius * sin(theta);

        // Верхняя вершина
        Vertex topVertex;
        topVertex.Position[0] = x;
        topVertex.Position[1] = halfHeight;
        topVertex.Position[2] = z;
        topVertex.Normal[0] = cos(theta);
        topVertex.Normal[1] = 0.0f;
        topVertex.Normal[2] = sin(theta);
        topVertex.TexCoords[0] = (float)i / (float)segments;
        topVertex.TexCoords[1] = 1.0f;
        vertices.push_back(topVertex);

        // Нижняя вершина
        Vertex bottomVertex;
        bottomVertex.Position[0] = x;
        bottomVertex.Position[1] = -halfHeight;
        bottomVertex.Position[2] = z;
        bottomVertex.Normal[0] = cos(theta);
        bottomVertex.Normal[1] = 0.0f;
        bottomVertex.Normal[2] = sin(theta);
        bottomVertex.TexCoords[0] = (float)i / (float)segments;
        bottomVertex.TexCoords[1] = 0.0f;
        vertices.push_back(bottomVertex);
    }

    // Индексы для боковой поверхности
    for (int i = 0; i < segments; ++i) {
        int topLeft = i * 2;
        int topRight = topLeft + 2;
        int bottomLeft = topLeft + 1;
        int bottomRight = topRight + 1;

        indices.push_back(topLeft);
        indices.push_back(bottomLeft);
        indices.push_back(topRight);

        indices.push_back(topRight);
        indices.push_back(bottomLeft);
        indices.push_back(bottomRight);
    }

    // Верхняя крышка
    int centerTop = (int)vertices.size();
    Vertex centerTopVertex;
    centerTopVertex.Position[0] = 0.0f;
    centerTopVertex.Position[1] = halfHeight;
    centerTopVertex.Position[2] = 0.0f;
    centerTopVertex.Normal[0] = 0.0f;
    centerTopVertex.Normal[1] = 1.0f;
    centerTopVertex.Normal[2] = 0.0f;
    centerTopVertex.TexCoords[0] = 0.5f;
    centerTopVertex.TexCoords[1] = 0.5f;
    vertices.push_back(centerTopVertex);

    // Нижняя крышка
    int centerBottom = (int)vertices.size();
    Vertex centerBottomVertex;
    centerBottomVertex.Position[0] = 0.0f;
    centerBottomVertex.Position[1] = -halfHeight;
    centerBottomVertex.Position[2] = 0.0f;
    centerBottomVertex.Normal[0] = 0.0f;
    centerBottomVertex.Normal[1] = -1.0f;
    centerBottomVertex.Normal[2] = 0.0f;
    centerBottomVertex.TexCoords[0] = 0.5f;
    centerBottomVertex.TexCoords[1] = 0.5f;
    vertices.push_back(centerBottomVertex);

    // Вершины для крышек
    for (int i = 0; i <= segments; ++i) {
        float theta = (float)i / (float)segments * 2.0f * PI;
        float x = radius * cos(theta);
        float z = radius * sin(theta);

        // Верхняя крышка
        Vertex topCap;
        topCap.Position[0] = x;
        topCap.Position[1] = halfHeight;
        topCap.Position[2] = z;
        topCap.Normal[0] = 0.0f;
        topCap.Normal[1] = 1.0f;
        topCap.Normal[2] = 0.0f;
        topCap.TexCoords[0] = (cos(theta) + 1.0f) * 0.5f;
        topCap.TexCoords[1] = (sin(theta) + 1.0f) * 0.5f;
        vertices.push_back(topCap);

        // Нижняя крышка
        Vertex bottomCap;
        bottomCap.Position[0] = x;
        bottomCap.Position[1] = -halfHeight;
        bottomCap.Position[2] = z;
        bottomCap.Normal[0] = 0.0f;
        bottomCap.Normal[1] = -1.0f;
        bottomCap.Normal[2] = 0.0f;
        bottomCap.TexCoords[0] = (cos(theta) + 1.0f) * 0.5f;
        bottomCap.TexCoords[1] = (sin(theta) + 1.0f) * 0.5f;
        vertices.push_back(bottomCap);
    }

    // Индексы для крышек
    for (int i = 0; i < segments; ++i) {
        int topStart = centerTop + 1;
        int bottomStart = centerBottom + 1;

        // Верхняя крышка
        indices.push_back(centerTop);
        indices.push_back(topStart + i + 1);
        indices.push_back(topStart + i);

        // Нижняя крышка
        indices.push_back(centerBottom);
        indices.push_back(bottomStart + i);
        indices.push_back(bottomStart + i + 1);
    }

    return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> Primitives::CreateCone(int segments) {
    // Простая заглушка - цилиндр с одной точкой
    return CreateCylinder(segments);
}

std::shared_ptr<Mesh> Primitives::CreatePyramid() {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float pyramidVertices[] = {
        // Позиции           // Нормали           // Текстурные координаты
        -0.5f, 0.0f, -0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
         0.5f, 0.0f, -0.5f,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
         0.5f, 0.0f,  0.5f,  0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
        -0.5f, 0.0f,  0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f,

        0.0f, 1.0f,  0.0f,  0.0f, 0.8f, 0.0f,   0.5f, 0.5f
    };

    unsigned int pyramidIndices[] = {
        0, 1, 2, 0, 2, 3,
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4
    };

    for (int i = 0; i < 5 * 8; i += 8) {
        Vertex vertex;
        vertex.Position[0] = pyramidVertices[i];
        vertex.Position[1] = pyramidVertices[i + 1];
        vertex.Position[2] = pyramidVertices[i + 2];
        vertex.Normal[0] = pyramidVertices[i + 3];
        vertex.Normal[1] = pyramidVertices[i + 4];
        vertex.Normal[2] = pyramidVertices[i + 5];
        vertex.TexCoords[0] = pyramidVertices[i + 6];
        vertex.TexCoords[1] = pyramidVertices[i + 7];
        vertices.push_back(vertex);
    }

    for (int i = 0; i < 18; i++) {
        indices.push_back(pyramidIndices[i]);
    }

    return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> Primitives::CreatePlane() {
    return CreateGrid(1);
}