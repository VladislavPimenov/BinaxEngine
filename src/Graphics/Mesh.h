#pragma once
#include <vector>
#include <GL/glew.h>

struct Vertex {
    float Position[3];
    float Normal[3];
    float TexCoords[2];
};

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices);
    ~Mesh();

    void Draw() const;

private:
    GLuint VAO = 0, VBO = 0, EBO = 0;
    size_t m_IndexCount = 0;

    void SetupMesh(const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices);
};