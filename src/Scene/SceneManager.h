#pragma once
#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "GameObject.h"
#include "Graphics/Shader.h"

class SceneManager {
public:
    SceneManager();
    ~SceneManager();

    void Initialize();
    void Update(float deltaTime);
    void Render(Shader& shader);
    void RenderDepth(Shader& depthShader);
    void RenderOutline(Shader& outlineShader, const glm::mat4& view, const glm::mat4& projection, 
                       const glm::vec3& color, int mode, float pointSize, float fillAlpha);

    std::shared_ptr<GameObject> CreateGameObject(const std::string& name);
    void DeleteGameObject(GameObject* object);
    void DuplicateSelectedObject();

    void SetSelectedObject(std::shared_ptr<GameObject> object);
    std::shared_ptr<GameObject> GetSelectedObject() const { return m_SelectedObject; }

    const std::vector<std::shared_ptr<GameObject>>& GetObjects() const { return m_Objects; }

    void SaveScene(const std::string& filename);
    void LoadScene(const std::string& filename);
    bool HasDirectionalLight() const;

    void SetActiveCamera(std::shared_ptr<GameObject> camera);
    std::shared_ptr<GameObject> GetActiveCamera() const { return m_ActiveCamera; }
    
    // Управление активной камерой (для редактора)
    void MoveActiveCamera(float forwardBack, float leftRight, float upDown, float speed);
    void RotateActiveCamera(float yawDelta, float pitchDelta);
    void UpdateActiveCamera(float deltaTime); // для плавности

    void RenderGrid(Shader& shader, const glm::mat4& view, const glm::mat4& projection);
    std::shared_ptr<Mesh> m_GridMesh;

    // Physics
    void InitializePhysics();
    void UpdatePhysics(float deltaTime);
    void SetPhysicsActive(bool active);
    void ResetPhysics();
    void RegisterForPhysicsReset(GameObject* obj);

private:
    bool m_Initialized;
    std::vector<std::shared_ptr<GameObject>> m_Objects;
    std::shared_ptr<GameObject> m_SelectedObject;
    std::shared_ptr<GameObject> m_ActiveCamera;
    float m_CameraYaw = -90.0f;
    float m_CameraPitch = 0.0f;
};
