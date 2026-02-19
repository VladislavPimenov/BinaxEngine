#pragma once
#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "imgui.h"
#include "ImGuizmo.h"

// Forward declarations
class SceneManager;
class GameObject;

struct EditorSettings {
    bool show_gizmo = true;
    bool wireframe_mode = false;
    bool grid_enabled = true;
    bool snap_to_grid = false;
    float grid_size = 1.0f;
    float bg_color[3] = {0.1f, 0.1f, 0.1f};
    float shininess = 32.0f;
    float metallic = 0.1f;
    float roughness = 0.3f;
    float light_intensity = 1.0f;
    glm::vec3 light_pos = glm::vec3(2.0f, 4.0f, 2.0f);
    glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f);
    bool shadows_enabled = true;
    float shadow_bias = 0.005f;
};

class EditorUI {
public:
    EditorUI();
    ~EditorUI();

    bool Initialize(GLFWwindow* window, SceneManager* sceneManager);
    void Shutdown();
    void BeginFrame();
    void EndFrame();
    void Render();

    const EditorSettings& GetSettings() const { return m_Settings; }
    EditorSettings& GetSettings() { return m_Settings; }

    std::shared_ptr<GameObject> GetSelectedObject() const;
    void SetSelectedObject(std::shared_ptr<GameObject> obj);

    void HandleShortcuts();

    // ===== ImGuizmo =====
    void SetViewProjection(const glm::mat4& view, const glm::mat4& projection);
    bool IsViewportHovered() const { return m_ViewportHovered; }
    GLFWwindow* GetWindow() const { return m_Window; }

private:
    void SetupImGuiStyle();
    void DrawMainMenuBar();
    void DrawHierarchy();
    void DrawInspector();
    void DrawSceneView();
    void DrawSceneSettings();
    void DrawContentBrowser();
    void DrawMaterialSettings();
    void DrawTransformControls(std::shared_ptr<GameObject> obj);
    void DrawObjectTreeNode(std::shared_ptr<GameObject> obj, int& id);
    void DrawGizmoToolbar();

private:
    GLFWwindow* m_Window = nullptr;
    SceneManager* m_SceneManager = nullptr;
    ImGuiContext* m_ImGuiContext = nullptr;
    EditorSettings m_Settings;

    bool m_ShowDemoWindow = false;
    bool m_ShowMetricsWindow = false;
    bool m_ShowAboutPopup = false;

    float m_MenuBarHeight = 0.0f;
    ImVec2 m_ViewportSize;
    ImVec2 m_ViewportPos;
    bool m_ViewportHovered = false;
    bool m_ViewportFocused = false;

    // ===== ImGuizmo =====
    glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
    glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
    ImGuizmo::OPERATION m_CurrentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGuizmo::MODE m_CurrentGizmoMode = ImGuizmo::WORLD;
};