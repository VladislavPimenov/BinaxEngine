#pragma once
#include <vector>
#include <memory>
#include <string>
#include "GameObject.h"
#include "Graphics/Shader.h"

class SceneManager {
public:
    SceneManager();
    ~SceneManager();

    void Initialize();
    void Update(float deltaTime);
    void Render(Shader& shader);          // рендерит все видимые объекты
    // void RenderOutlines() - можно оставить, но пока не используем
    // void RenderGizmos(Shader& shader) - УДАЛЕНО, гизмо теперь рисует ImGuizmo в редакторе

    std::shared_ptr<GameObject> CreateGameObject(const std::string& name);
    void DeleteGameObject(GameObject* object);
    void DuplicateSelectedObject();

    void SetSelectedObject(std::shared_ptr<GameObject> object);
    std::shared_ptr<GameObject> GetSelectedObject() const { return m_SelectedObject; }

    const std::vector<std::shared_ptr<GameObject>>& GetObjects() const { return m_Objects; }

    void SaveScene(const std::string& filename);
    void LoadScene(const std::string& filename);

private:
    bool m_Initialized;
    std::vector<std::shared_ptr<GameObject>> m_Objects;
    std::shared_ptr<GameObject> m_SelectedObject;
};