#include "Scene/GameObject.h"
#include "Graphics/Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <GL/glew.h>
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

// ===== ИСПРАВЛЕННАЯ ФУНКЦИЯ ВРАЩЕНИЯ =====
void GameObject::SetRotation(const glm::vec3& rotation) {
    // Сохраняем предыдущее вращение
    m_PreviousRotation = m_Rotation;
    
    // Применяем новое вращение
    m_Rotation = rotation;
    
    // Нормализуем углы (в диапазон -180 до 180)
    while (m_Rotation.x > 180.0f) m_Rotation.x -= 360.0f;
    while (m_Rotation.x < -180.0f) m_Rotation.x += 360.0f;
    while (m_Rotation.y > 180.0f) m_Rotation.y -= 360.0f;
    while (m_Rotation.y < -180.0f) m_Rotation.y += 360.0f;
    while (m_Rotation.z > 180.0f) m_Rotation.z -= 360.0f;
    while (m_Rotation.z < -180.0f) m_Rotation.z += 360.0f;
}
// ==========================================

void GameObject::SetScale(const glm::vec3& scale) {
    m_Scale = scale;
    if (m_Scale.x == 0) m_Scale.x = 0.001f;
    if (m_Scale.y == 0) m_Scale.y = 0.001f;
    if (m_Scale.z == 0) m_Scale.z = 0.001f;
}

void GameObject::Draw(Shader& shader) {
    if (!m_Visible || !m_Mesh) return;
    glm::mat4 transform = GetTransformMatrix();
    shader.SetMat4("model", &transform[0][0]);
    shader.SetVec3("objectColor", m_Color.x, m_Color.y, m_Color.z);
    m_Mesh->Draw();
}

void GameObject::DrawOutline() {
    if (!m_Visible || !m_Mesh) return;
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(1.0f, 1.0f, 0.0f);
    glLineWidth(3.0f);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glm::mat4 transform = GetTransformMatrix();
    float matrix[16];
    const float* src = &transform[0][0];
    for (int i = 0; i < 16; i++) {
        matrix[i] = src[i];
    }
    glLoadMatrixf(matrix);
    glPopMatrix();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(1.0f);
}

void GameObject::DrawGizmo() {
    if (!m_GizmoVisible || !m_Visible) return;
    switch (m_GizmoMode) {
        case GizmoMode::TRANSLATE:
            DrawTranslateGizmo();
            break;
        case GizmoMode::ROTATE:
            DrawRotateGizmo();
            break;
        case GizmoMode::SCALE:
            DrawScaleGizmo();
            break;
        default:
            break;
    }
}

void GameObject::DrawTranslateGizmo() {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glm::mat4 transform = GetTransformMatrix();
    float matrix[16];
    const float* src = &transform[0][0];
    for (int i = 0; i < 16; i++) {
        matrix[i] = src[i];
    }
    glLoadMatrixf(matrix);

    // Ось X (красная)
    glBegin(GL_LINES);
    if (m_SelectedGizmoAxis == GizmoAxis::X || m_SelectedGizmoAxis == GizmoAxis::ALL) {
        glColor3f(1.0f, 0.5f, 0.0f);
    } else {
        glColor3f(1.0f, 0.0f, 0.0f);
    }
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glEnd();

    // Ось Y (зеленая)
    glBegin(GL_LINES);
    if (m_SelectedGizmoAxis == GizmoAxis::Y || m_SelectedGizmoAxis == GizmoAxis::ALL) {
        glColor3f(0.5f, 1.0f, 0.0f);
    } else {
        glColor3f(0.0f, 1.0f, 0.0f);
    }
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();

    // Ось Z (синяя)
    glBegin(GL_LINES);
    if (m_SelectedGizmoAxis == GizmoAxis::Z || m_SelectedGizmoAxis == GizmoAxis::ALL) {
        glColor3f(0.0f, 0.5f, 1.0f);
    } else {
        glColor3f(0.0f, 0.0f, 1.0f);
    }
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();

    // Кубы на концах осей
    DrawCubeGizmo(1.0f, 0.0f, 0.0f, 0.1f, 
                  m_SelectedGizmoAxis == GizmoAxis::X ? 1.0f : 0.7f,
                  m_SelectedGizmoAxis == GizmoAxis::X ? 0.5f : 0.0f, 0.0f);
    DrawCubeGizmo(0.0f, 1.0f, 0.0f, 0.1f, 0.0f,
                  m_SelectedGizmoAxis == GizmoAxis::Y ? 1.0f : 0.7f, 0.0f);
    DrawCubeGizmo(0.0f, 0.0f, 1.0f, 0.1f, 0.0f, 0.0f,
                  m_SelectedGizmoAxis == GizmoAxis::Z ? 1.0f : 0.7f);
    DrawCubeGizmo(0.0f, 0.0f, 0.0f, 0.15f,
                  m_SelectedGizmoAxis == GizmoAxis::ALL ? 1.0f : 0.8f,
                  m_SelectedGizmoAxis == GizmoAxis::ALL ? 1.0f : 0.8f,
                  m_SelectedGizmoAxis == GizmoAxis::ALL ? 1.0f : 0.8f);

    glPopMatrix();
}

void GameObject::DrawScaleGizmo() {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glm::mat4 transform = GetTransformMatrix();
    float matrix[16];
    const float* src = &transform[0][0];
    for (int i = 0; i < 16; i++) {
        matrix[i] = src[i];
    }
    glLoadMatrixf(matrix);

    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();

    float cubeSize = 0.15f;
    DrawCubeGizmo(1.0f, 0.0f, 0.0f, cubeSize, 1.0f, 0.0f, 0.0f);
    DrawCubeGizmo(0.0f, 1.0f, 0.0f, cubeSize, 0.0f, 1.0f, 0.0f);
    DrawCubeGizmo(0.0f, 0.0f, 1.0f, cubeSize, 0.0f, 0.0f, 1.0f);
    DrawCubeGizmo(0.0f, 0.0f, 0.0f, 0.2f, 1.0f, 1.0f, 1.0f);

    glPopMatrix();
}

void GameObject::DrawRotateGizmo() {
    DrawTranslateGizmo();
}

void GameObject::DrawCubeGizmo(float x, float y, float z, float size, float r, float g, float b) {
    glColor3f(r, g, b);
    glPushMatrix();
    glTranslatef(x, y, z);
    float half = size / 2.0f;

    float vertices[8][3] = {
        {-half, -half, -half}, { half, -half, -half},
        { half,  half, -half}, {-half,  half, -half},
        {-half, -half,  half}, { half, -half,  half},
        { half,  half,  half}, {-half,  half,  half}
    };

    int indices[12][2] = {
        {0,1}, {1,2}, {2,3}, {3,0},
        {4,5}, {5,6}, {6,7}, {7,4},
        {0,4}, {1,5}, {2,6}, {3,7}
    };

    glBegin(GL_LINES);
    for (int i = 0; i < 12; i++) {
        glVertex3fv(vertices[indices[i][0]]);
        glVertex3fv(vertices[indices[i][1]]);
    }
    glEnd();
    glPopMatrix();
}

void GameObject::SetSelectedGizmoAxis(GizmoAxis axis) {
    m_SelectedGizmoAxis = axis;
}