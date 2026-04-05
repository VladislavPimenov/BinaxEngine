#include "Graphics/Material.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Material::Material() {}

Material::~Material() {
    if (m_DiffuseTexture) glDeleteTextures(1, &m_DiffuseTexture);
    if (m_NormalTexture) glDeleteTextures(1, &m_NormalTexture);
    if (m_RoughnessTexture) glDeleteTextures(1, &m_RoughnessTexture);
    if (m_MetallicTexture) glDeleteTextures(1, &m_MetallicTexture);
    if (m_AOTexture) glDeleteTextures(1, &m_AOTexture);
}

bool Material::LoadDiffuseTexture(const std::string& path) {
    if (m_DiffuseTexture) glDeleteTextures(1, &m_DiffuseTexture);
    m_DiffuseTexture = LoadTexture(path);
    return m_DiffuseTexture != 0;
}

bool Material::LoadNormalTexture(const std::string& path) {
    if (m_NormalTexture) glDeleteTextures(1, &m_NormalTexture);
    m_NormalTexture = LoadTexture(path);
    return m_NormalTexture != 0;
}

bool Material::LoadRoughnessTexture(const std::string& path) {
    if (m_RoughnessTexture) glDeleteTextures(1, &m_RoughnessTexture);
    m_RoughnessTexture = LoadTexture(path);
    return m_RoughnessTexture != 0;
}

bool Material::LoadMetallicTexture(const std::string& path) {
    if (m_MetallicTexture) glDeleteTextures(1, &m_MetallicTexture);
    m_MetallicTexture = LoadTexture(path);
    return m_MetallicTexture != 0;
}

bool Material::LoadAOTexture(const std::string& path) {
    if (m_AOTexture) glDeleteTextures(1, &m_AOTexture);
    m_AOTexture = LoadTexture(path);
    return m_AOTexture != 0;
}

void Material::BindTextures() const {
    if (m_DiffuseTexture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_DiffuseTexture);
    }
    if (m_NormalTexture) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_NormalTexture);
    }
    if (m_RoughnessTexture) {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, m_RoughnessTexture);
    }
    if (m_MetallicTexture) {
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, m_MetallicTexture);
    }
    if (m_AOTexture) {
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, m_AOTexture);
    }
}

void Material::UnbindTextures() const {
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE4); glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE5); glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint Material::LoadTexture(const std::string& path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    if (textureID == 0) {
        std::cerr << "[Material] Failed to generate texture ID" << std::endl;
        return 0;
    }

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
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
        return textureID;
    } else {
        std::cerr << "[Material] Failed to load texture: " << path << " - " << stbi_failure_reason() << std::endl;
        glDeleteTextures(1, &textureID);
        return 0;
    }
}
