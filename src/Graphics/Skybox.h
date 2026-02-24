#pragma once
#include <string>
#include <GL/glew.h>

class Skybox {
public:
    Skybox();
    ~Skybox();

    bool Load(const std::string& right, const std::string& left,
              const std::string& top, const std::string& bottom,
              const std::string& front, const std::string& back);
    void Draw() const;

private:
    GLuint m_VAO = 0;
    GLuint m_VBO = 0;
    GLuint m_TextureID = 0;
};