#pragma once
#include <vector>
#include <string>
#include <GL/glew.h>

struct Vertex {
    float Position[3];
    float Normal[3];
    float TexCoords[2];
    float Tangent[3];   // касательная
};

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices,
         const std::vector<unsigned int>& indices,
         const std::string& diffusePath = "",
         const std::string& normalPath = "");
    ~Mesh();

    void Draw() const;

private:
    GLuint VAO = 0, VBO = 0, EBO = 0;
    size_t m_IndexCount = 0;
    GLuint m_DiffuseTexture = 0;
    GLuint m_NormalTexture = 0;

    void SetupMesh(const std::vector<Vertex>& vertices,
                   const std::vector<unsigned int>& indices);
    GLuint LoadTexture(const std::string& path);
};