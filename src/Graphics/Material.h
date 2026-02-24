#pragma once
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Material {
public:
    Material();
    ~Material();

    bool LoadDiffuseTexture(const std::string& path);
    bool LoadNormalTexture(const std::string& path);
    void BindTextures() const;
    void UnbindTextures() const;

    bool HasDiffuse() const { return m_DiffuseTexture != 0; }
    bool HasNormal() const  { return m_NormalTexture != 0; }

    // Параметры материала
    glm::vec3 albedo = glm::vec3(1.0f);
    float metallic = 0.0f;        // ← добавили инициализацию
    float roughness = 0.5f;        // ← добавили инициализацию
    float ao = 1.0f;

    glm::vec2 uvScale = glm::vec2(1.0f);
    float normalStrength = 1.0f;

private:
    GLuint m_DiffuseTexture = 0;
    GLuint m_NormalTexture = 0;
    GLuint LoadTexture(const std::string& path);
};