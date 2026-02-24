#include "Scene/GameObject.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

GameObject::GameObject(const std::string& name)
    : m_Name(name) {
}

GameObject::~GameObject() {
    for (auto& child : m_Children) {
        child->m_Parent = nullptr;
    }
}

void GameObject::AddChild(std::shared_ptr<GameObject> child) {
    if (child->m_Parent == this) return;
    if (child->m_Parent) {
        child->m_Parent->RemoveChild(child.get());
    }
    child->m_Parent = this;
    m_Children.push_back(child);
}

void GameObject::RemoveChild(GameObject* child) {
    for (auto it = m_Children.begin(); it != m_Children.end(); ++it) {
        if (it->get() == child) {
            child->m_Parent = nullptr;
            m_Children.erase(it);
            break;
        }
    }
}

glm::vec3 GameObject::GetWorldPosition() const {
    if (m_Parent) {
        return m_Parent->GetWorldPosition() + m_Position;
    }
    return m_Position;
}

glm::mat4 GameObject::GetTransformMatrix() const {
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, m_Position);
    glm::mat4 rotation = glm::eulerAngleYXZ(
        glm::radians(m_Rotation.y),
        glm::radians(m_Rotation.x),
        glm::radians(m_Rotation.z)
    );
    transform = transform * rotation;
    transform = glm::scale(transform, m_Scale);
    if (m_Parent) {
        transform = m_Parent->GetTransformMatrix() * transform;
    }
    return transform;
}

void GameObject::SetPosition(const glm::vec3& position) {
    m_Position = position;
}

void GameObject::SetRotation(const glm::vec3& rotation) {
    m_PreviousRotation = m_Rotation;
    m_Rotation = rotation;
    auto normalize = [](float angle) {
        while (angle > 180.0f) angle -= 360.0f;
        while (angle < -180.0f) angle += 360.0f;
        return angle;
    };
    m_Rotation.x = normalize(m_Rotation.x);
    m_Rotation.y = normalize(m_Rotation.y);
    m_Rotation.z = normalize(m_Rotation.z);
}

void GameObject::SetScale(const glm::vec3& scale) {
    m_Scale = scale;
    if (m_Scale.x == 0.0f) m_Scale.x = 0.001f;
    if (m_Scale.y == 0.0f) m_Scale.y = 0.001f;
    if (m_Scale.z == 0.0f) m_Scale.z = 0.001f;
}

void GameObject::Draw(Shader& shader) const {
    std::cout << "  Drawing " << m_Name << ", material: " << (m_Material ? "yes" : "no") << std::endl;
    if (!m_Visible || !m_Mesh) return;

    glm::mat4 transform = GetTransformMatrix();
    shader.SetMat4("model", glm::value_ptr(transform));

    if (m_Material) {
        shader.SetBool("hasDiffuseTexture", true);
        shader.SetBool("hasNormalMap", m_Material->HasNormal());
        shader.SetFloat("shininess", 32.0f); // пока оставим, но можно заменить на roughness

        // ===== НОВЫЕ ПАРАМЕТРЫ =====
        shader.SetFloat("metallic", m_Material->metallic);
        shader.SetFloat("roughness", m_Material->roughness);
        // ============================

        shader.SetVec2("uvScale", m_Material->uvScale.x, m_Material->uvScale.y);
        shader.SetFloat("normalStrength", m_Material->normalStrength);

        m_Material->BindTextures();
        shader.SetInt("diffuseTexture", 0);
        shader.SetInt("normalMap", 1);
    } else {
        shader.SetBool("hasDiffuseTexture", false);
        shader.SetBool("hasNormalMap", false);
        shader.SetVec3("objectColor", m_Color.x, m_Color.y, m_Color.z);
    }

    m_Mesh->Draw();

    if (m_Material) {
        m_Material->UnbindTextures();
    }
}