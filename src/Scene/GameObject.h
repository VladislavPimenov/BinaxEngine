#pragma once
#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Graphics/Shader.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"   // <-- добавили

class GameObject {
public:
    GameObject(const std::string& name = "GameObject");
    ~GameObject();

    // Иерархия
    void AddChild(std::shared_ptr<GameObject> child);
    void RemoveChild(GameObject* child);
    GameObject* GetParent() const { return m_Parent; }
    const std::vector<std::shared_ptr<GameObject>>& GetChildren() const { return m_Children; }

    // Трансформация
    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::vec3& rotation);
    void SetScale(const glm::vec3& scale);
    glm::vec3 GetPosition() const { return m_Position; }
    glm::vec3 GetRotation() const { return m_Rotation; }
    glm::vec3 GetScale() const { return m_Scale; }
    glm::vec3 GetWorldPosition() const;
    glm::mat4 GetTransformMatrix() const;

    // Меш и видимость
    void SetMesh(std::shared_ptr<Mesh> mesh) { m_Mesh = mesh; }
    std::shared_ptr<Mesh> GetMesh() const { return m_Mesh; }
    void SetVisible(bool visible) { m_Visible = visible; }
    bool IsVisible() const { return m_Visible; }

    // Имя и цвет
    void SetName(const std::string& name) { m_Name = name; }
    std::string GetName() const { return m_Name; }
    void SetColor(const glm::vec3& color) { m_Color = color; }
    glm::vec3 GetColor() const { return m_Color; }

    // ===== Материал =====
    void SetMaterial(std::shared_ptr<Material> mat) { m_Material = mat; }
    std::shared_ptr<Material> GetMaterial() const { return m_Material; }

    // Отрисовка
    void Draw(Shader& shader) const;

private:
    std::string m_Name;
    GameObject* m_Parent = nullptr;
    std::vector<std::shared_ptr<GameObject>> m_Children;

    glm::vec3 m_Position = glm::vec3(0.0f);
    glm::vec3 m_Rotation = glm::vec3(0.0f);
    glm::vec3 m_Scale = glm::vec3(1.0f);
    std::shared_ptr<Mesh> m_Mesh;
    glm::vec3 m_Color = glm::vec3(1.0f);
    bool m_Visible = true;

    std::shared_ptr<Material> m_Material;   // <-- новое поле

    glm::vec3 m_PreviousRotation = glm::vec3(0.0f);
};