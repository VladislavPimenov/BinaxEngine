#include "Scene/SceneManager.h"
#include "Graphics/Primitives.h"
#include <iostream>
#include <fstream>

SceneManager::SceneManager() {
    m_Initialized = false;
}

SceneManager::~SceneManager() {
    m_Objects.clear();
    m_SelectedObject.reset();
}

void SceneManager::Initialize() {
    if (m_Initialized) return;
    std::cout << "Initializing SceneManager..." << std::endl;

    auto grid = CreateGameObject("Grid");
    grid->SetMesh(Primitives::CreateGrid(10));
    grid->SetPosition(glm::vec3(0.0f, -0.5f, 0.0f));
    grid->SetColor(glm::vec3(0.3f, 0.3f, 0.3f));
    grid->SetGizmoVisible(false);

    auto cube = CreateGameObject("Cube");
    cube->SetMesh(Primitives::CreateCube());
    cube->SetPosition(glm::vec3(0.0f, 0.5f, 0.0f));
    cube->SetColor(glm::vec3(0.8f, 0.3f, 0.2f));

    auto light = CreateGameObject("Light");
    light->SetMesh(Primitives::CreateCube());
    light->SetPosition(glm::vec3(2.0f, 3.0f, 2.0f));
    light->SetScale(glm::vec3(0.2f));
    light->SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
    light->SetGizmoVisible(false);

    SetSelectedObject(cube);
    m_Initialized = true;
    std::cout << "SceneManager initialized with " << m_Objects.size() << " objects" << std::endl;
}

void SceneManager::SetSelectedObject(std::shared_ptr<GameObject> object) {
    if (m_SelectedObject) {
        m_SelectedObject->SetGizmoSelected(false);
    }
    m_SelectedObject = object;
    if (m_SelectedObject) {
        m_SelectedObject->SetGizmoSelected(true);
    }
}

std::shared_ptr<GameObject> SceneManager::CreateGameObject(const std::string& name) {
    auto obj = std::make_shared<GameObject>(name);
    m_Objects.push_back(obj);
    return obj;
}

void SceneManager::DeleteGameObject(GameObject* object) {
    for (auto it = m_Objects.begin(); it != m_Objects.end(); ++it) {
        if (it->get() == object) {
            if (m_SelectedObject.get() == object) {
                m_SelectedObject.reset();
            }
            m_Objects.erase(it);
            break;
        }
    }
}

void SceneManager::DuplicateSelectedObject() {
    if (!m_SelectedObject) return;
    auto newObj = CreateGameObject(m_SelectedObject->GetName() + " (Copy)");
    newObj->SetMesh(m_SelectedObject->GetMesh());
    newObj->SetPosition(m_SelectedObject->GetPosition() + glm::vec3(1.0f, 0.0f, 0.0f));
    newObj->SetRotation(m_SelectedObject->GetRotation());
    newObj->SetScale(m_SelectedObject->GetScale());
    newObj->SetColor(m_SelectedObject->GetColor());
    SetSelectedObject(newObj);
}

void SceneManager::Update(float deltaTime) {
    // Обновляем логику объектов
}

void SceneManager::Render(Shader& shader) {
    if (!m_Initialized) return;
    for (size_t i = 0; i < m_Objects.size(); i++) {
        const auto& obj = m_Objects[i];
        if (!obj->IsVisible()) continue;
        obj->Draw(shader);
    }
}

void SceneManager::RenderOutlines() {
    if (!m_Initialized) return;
    if (m_SelectedObject && m_SelectedObject->IsVisible()) {
        m_SelectedObject->DrawOutline();
    }
}

void SceneManager::RenderGizmos(Shader& shader) {  // ← Параметр shader оставлен для совместимости
    if (!m_Initialized) return;
    if (m_SelectedObject && m_SelectedObject->IsVisible()) {
        m_SelectedObject->DrawGizmo();  // ← Вызов без параметров (как в header)
    }
}

void SceneManager::SaveScene(const std::string& filename) {
    std::cout << "Сохранение сцены в: " << filename << std::endl;
}

void SceneManager::LoadScene(const std::string& filename) {
    std::cout << "Загрузка сцены из: " << filename << std::endl;
}