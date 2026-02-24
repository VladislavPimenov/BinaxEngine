#include "Scene/SceneManager.h"
#include "Graphics/Primitives.h"
#include "Graphics/Material.h"
#include <iostream>
#include <fstream>
#include <memory>

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

    std::cout << "Step 1: Creating grid..." << std::endl;
    auto grid = CreateGameObject("Grid");
    grid->SetMesh(Primitives::CreateGrid(10));
    grid->SetPosition(glm::vec3(0.0f, -0.5f, 0.0f));
    grid->SetColor(glm::vec3(0.3f, 0.3f, 0.3f));

    std::cout << "Step 2: Creating cube..." << std::endl;
    auto cube = CreateGameObject("Cube");
    cube->SetMesh(Primitives::CreateCube());
    cube->SetPosition(glm::vec3(0.0f, 0.5f, 0.0f));
    cube->SetColor(glm::vec3(0.8f, 0.3f, 0.2f));

    std::cout << "Step 3: Creating light..." << std::endl;
    auto light = CreateGameObject("Light");
    light->SetMesh(Primitives::CreateCube());
    light->SetPosition(glm::vec3(2.0f, 3.0f, 2.0f));
    light->SetScale(glm::vec3(0.2f));
    light->SetColor(glm::vec3(1.0f, 1.0f, 1.0f));

    std::cout << "Step 4: Creating textured cube..." << std::endl;
    auto texCube = CreateGameObject("TexturedCube");   // ← вот это было пропущено
    texCube->SetMesh(Primitives::CreateCube());
    texCube->SetPosition(glm::vec3(2.0f, 0.5f, 0.0f));

    std::cout << "Step 5: Creating material..." << std::endl;
    auto material = std::make_shared<Material>();

    std::cout << "Step 6: Loading diffuse texture..." << std::endl;
    if (!material->LoadDiffuseTexture("assets/textures/wood_diffuse.jpg")) {
        std::cerr << "Failed to load diffuse texture" << std::endl;
    }

    std::cout << "Step 7: Loading normal texture..." << std::endl;
    if (!material->LoadNormalTexture("assets/textures/wood_normal.jpg")) {
        std::cerr << "Failed to load normal texture" << std::endl;
    }

    std::cout << "Step 8: Setting material..." << std::endl;
    texCube->SetMaterial(material);

    SetSelectedObject(cube);
    m_Initialized = true;
    std::cout << "SceneManager initialized with " << m_Objects.size() << " objects" << std::endl;
}

void SceneManager::SetSelectedObject(std::shared_ptr<GameObject> object) {
    m_SelectedObject = object;
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
    if (m_SelectedObject->GetMaterial()) {
        newObj->SetMaterial(m_SelectedObject->GetMaterial());
    }
    SetSelectedObject(newObj);
}

void SceneManager::Update(float deltaTime) {}

void SceneManager::Render(Shader& shader) {
    if (!m_Initialized) return;
    for (const auto& obj : m_Objects) {
        if (obj->IsVisible()) {
            obj->Draw(shader);
        }
    }
}

void SceneManager::SaveScene(const std::string& filename) {
    std::cout << "Saving scene to: " << filename << std::endl;
}

void SceneManager::LoadScene(const std::string& filename) {
    std::cout << "Loading scene from: " << filename << std::endl;
}