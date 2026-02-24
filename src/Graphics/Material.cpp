#include "Graphics/Material.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Material::Material() {}

Material::~Material() {
    if (m_DiffuseTexture) glDeleteTextures(1, &m_DiffuseTexture);
    if (m_NormalTexture) glDeleteTextures(1, &m_NormalTexture);
}

bool Material::LoadDiffuseTexture(const std::string& path) {
    std::cout << "[Material] LoadDiffuseTexture: " << path << std::endl;
    m_DiffuseTexture = LoadTexture(path);
    std::cout << "[Material] Diffuse texture ID: " << m_DiffuseTexture << std::endl;
    return m_DiffuseTexture != 0;
}

bool Material::LoadNormalTexture(const std::string& path) {
    std::cout << "[Material] LoadNormalTexture: " << path << std::endl;
    m_NormalTexture = LoadTexture(path);
    std::cout << "[Material] Normal texture ID: " << m_NormalTexture << std::endl;
    return m_NormalTexture != 0;
}

void Material::BindTextures() const {
    std::cout << "[Material] BindTextures: diffuse=" << m_DiffuseTexture << ", normal=" << m_NormalTexture << std::endl;
    if (m_DiffuseTexture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_DiffuseTexture);
    }
    if (m_NormalTexture) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_NormalTexture);
    }
}

void Material::UnbindTextures() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint Material::LoadTexture(const std::string& path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    if (textureID == 0) {
        std::cerr << "[Material] Failed to generate texture ID" << std::endl;
        return 0;
    }

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        std::cout << "[Material] Texture loaded successfully, ID=" << textureID << std::endl;
        return textureID;
    } else {
        std::cerr << "[Material] Failed to load texture: " << path << " - " << stbi_failure_reason() << std::endl;
        glDeleteTextures(1, &textureID);
        return 0;
    }
}