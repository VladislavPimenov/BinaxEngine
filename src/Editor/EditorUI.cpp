// GLEW must be included first
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Editor/EditorUI.h"
#include "Scene/SceneManager.h"
#include "Scene/GameObject.h"
#include "Graphics/Primitives.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuizmo.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/matrix_decompose.hpp>

// Глобальные переменные из main.cpp
extern bool mouseCaptured;
extern bool firstMouse;

EditorUI::EditorUI() {
    std::cout << "EditorUI created" << std::endl;
}

EditorUI::~EditorUI() {
    Shutdown();
}

bool EditorUI::Initialize(GLFWwindow* window, SceneManager* sceneManager) {
    m_Window = window;
    m_SceneManager = sceneManager;

    IMGUI_CHECKVERSION();
    m_ImGuiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(m_ImGuiContext);

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
        std::cerr << "Failed to initialize ImGui GLFW backend" << std::endl;
        return false;
    }

    if (!ImGui_ImplOpenGL3_Init("#version 130")) {
        std::cerr << "Failed to initialize ImGui OpenGL backend" << std::endl;
        return false;
    }

    SetupImGuiStyle();
    std::cout << "EditorUI initialized successfully" << std::endl;
    return true;
}

void EditorUI::Shutdown() {
    if (m_ImGuiContext) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext(m_ImGuiContext);
        m_ImGuiContext = nullptr;
    }
}

void EditorUI::SetupImGuiStyle() {
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
}

void EditorUI::BeginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ===== Инициализация ImGuizmo =====
    ImGuizmo::BeginFrame();
    ImGuizmo::SetImGuiContext(m_ImGuiContext);
    // ==================================
}

void EditorUI::EndFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void EditorUI::SetViewProjection(const glm::mat4& view, const glm::mat4& projection) {
    m_ViewMatrix = view;
    m_ProjectionMatrix = projection;
}

void EditorUI::Render() {
    DrawMainMenuBar();
    DrawHierarchy();
    DrawInspector();
    DrawSceneView();
    DrawSceneSettings();
    DrawContentBrowser();
    DrawMaterialSettings();

    if (m_ShowAboutPopup) {
        ImGui::OpenPopup("About");
        m_ShowAboutPopup = false;
    }

    if (ImGui::BeginPopupModal("About", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Binax Engine v0.2.0");
        ImGui::Text("A simple 3D game engine");
        ImGui::Separator();
        ImGui::Text("Created with OpenGL, GLFW and Dear ImGui");
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void EditorUI::DrawMainMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        m_MenuBarHeight = ImGui::GetWindowHeight();

        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
                if (m_SceneManager) m_SceneManager->SaveScene("scene.binax");
            }
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                glfwSetWindowShouldClose(m_Window, GLFW_TRUE);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Duplicate", "Ctrl+D")) {
                if (m_SceneManager) m_SceneManager->DuplicateSelectedObject();
            }
            if (ImGui::MenuItem("Delete", "Del")) {
                if (m_SceneManager) {
                    auto selected = m_SceneManager->GetSelectedObject();
                    if (selected) m_SceneManager->DeleteGameObject(selected.get());
                }
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("GameObject")) {
            if (ImGui::MenuItem("Create Empty")) {
                if (m_SceneManager) m_SceneManager->CreateGameObject("Empty");
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Cube")) {
                if (m_SceneManager) {
                    auto cube = m_SceneManager->CreateGameObject("Cube");
                    cube->SetMesh(Primitives::CreateCube());
                    cube->SetColor(glm::vec3(0.8f, 0.3f, 0.2f));
                }
            }
            if (ImGui::MenuItem("Sphere")) {
                if (m_SceneManager) {
                    auto sphere = m_SceneManager->CreateGameObject("Sphere");
                    sphere->SetMesh(Primitives::CreateSphere());
                    sphere->SetColor(glm::vec3(0.3f, 0.8f, 0.2f));
                }
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Wireframe Mode", "", &m_Settings.wireframe_mode);
            ImGui::MenuItem("Show Grid", "", &m_Settings.grid_enabled);
            ImGui::MenuItem("Show Gizmo", "", &m_Settings.show_gizmo);
            ImGui::EndMenu();
        }

        // ===== КНОПКА ЗАХВАТА МЫШИ =====
        ImGui::SameLine(ImGui::GetWindowWidth() - 350);
        DrawGizmoToolbar();
        ImGui::SameLine();

        if (mouseCaptured) {
            if (ImGui::Button("Release Mouse (ESC)")) {
                mouseCaptured = false;
                firstMouse = true;
                glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        } else {
            if (ImGui::Button("Capture Mouse")) {
                mouseCaptured = true;
                firstMouse = true;
                glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
        }

        ImGui::EndMainMenuBar();
    }
}

void EditorUI::DrawGizmoToolbar() {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.35f, 0.35f, 0.35f, 1.0f));

    if (m_CurrentGizmoOperation == ImGuizmo::TRANSLATE) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.6f, 1.0f));
    }
    if (ImGui::Button("T", ImVec2(30, 25))) {
        m_CurrentGizmoOperation = ImGuizmo::TRANSLATE;
    }
    if (m_CurrentGizmoOperation == ImGuizmo::TRANSLATE) {
        ImGui::PopStyleColor();
    }
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Translate (W)");

    ImGui::SameLine();

    if (m_CurrentGizmoOperation == ImGuizmo::ROTATE) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.6f, 1.0f));
    }
    if (ImGui::Button("R", ImVec2(30, 25))) {
        m_CurrentGizmoOperation = ImGuizmo::ROTATE;
    }
    if (m_CurrentGizmoOperation == ImGuizmo::ROTATE) {
        ImGui::PopStyleColor();
    }
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Rotate (E)");

    ImGui::SameLine();

    if (m_CurrentGizmoOperation == ImGuizmo::SCALE) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.6f, 1.0f));
    }
    if (ImGui::Button("S", ImVec2(30, 25))) {
        m_CurrentGizmoOperation = ImGuizmo::SCALE;
    }
    if (m_CurrentGizmoOperation == ImGuizmo::SCALE) {
        ImGui::PopStyleColor();
    }
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Scale (R)");

    ImGui::SameLine();

    const char* modeText = (m_CurrentGizmoMode == ImGuizmo::WORLD) ? "World" : "Local";
    if (ImGui::Button(modeText, ImVec2(50, 25))) {
        m_CurrentGizmoMode = (m_CurrentGizmoMode == ImGuizmo::WORLD) ? ImGuizmo::LOCAL : ImGuizmo::WORLD;
    }
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Toggle Local/World (X)");

    ImGui::PopStyleColor(3);
}

void EditorUI::DrawHierarchy() {
    if (ImGui::Begin("Hierarchy")) {
        ImGui::Text("Scene Objects");
        ImGui::Separator();

        if (m_SceneManager) {
            int id = 0;
            for (const auto& obj : m_SceneManager->GetObjects()) {
                DrawObjectTreeNode(obj, id);
            }

            ImGui::Separator();
            if (ImGui::Button("+ Add Object", ImVec2(-1, 0))) {
                ImGui::OpenPopup("AddObjectPopup");
            }

            if (ImGui::BeginPopup("AddObjectPopup")) {
                if (ImGui::MenuItem("Empty Object")) {
                    m_SceneManager->CreateGameObject("Empty");
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Cube")) {
                    auto cube = m_SceneManager->CreateGameObject("Cube");
                    cube->SetMesh(Primitives::CreateCube());
                    cube->SetColor(glm::vec3(0.8f, 0.3f, 0.2f));
                }
                if (ImGui::MenuItem("Sphere")) {
                    auto sphere = m_SceneManager->CreateGameObject("Sphere");
                    sphere->SetMesh(Primitives::CreateSphere());
                    sphere->SetColor(glm::vec3(0.3f, 0.8f, 0.2f));
                }
                ImGui::EndPopup();
            }
        }
    }
    ImGui::End();
}

void EditorUI::DrawObjectTreeNode(std::shared_ptr<GameObject> obj, int& id) {
    ImGui::PushID(id++);

    bool isSelected = (obj == m_SceneManager->GetSelectedObject());

    const char* icon = "[]";
    if (obj->GetName().find("Light") != std::string::npos) icon = "[L]";
    else if (obj->GetName().find("Grid") != std::string::npos) icon = "[G]";

    ImGui::Selectable((std::string(icon) + " " + obj->GetName()).c_str(), isSelected);

    if (ImGui::IsItemClicked()) {
        m_SceneManager->SetSelectedObject(obj);
    }

    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Duplicate", "Ctrl+D")) {
            m_SceneManager->DuplicateSelectedObject();
        }
        if (ImGui::MenuItem("Delete", "Del")) {
            m_SceneManager->DeleteGameObject(obj.get());
        }
        ImGui::EndPopup();
    }

    ImGui::PopID();
}

void EditorUI::DrawInspector() {
    if (ImGui::Begin("Inspector")) {
        auto selected = m_SceneManager ? m_SceneManager->GetSelectedObject() : nullptr;

        if (selected) {
            ImGui::Text("%s", selected->GetName().c_str());
            ImGui::Separator();

            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                DrawTransformControls(selected);
            }

            if (ImGui::CollapsingHeader("Appearance")) {
                glm::vec3 color = selected->GetColor();
                float colorArr[3] = { color.x, color.y, color.z };
                if (ImGui::ColorEdit3("Color", colorArr)) {
                    selected->SetColor(glm::vec3(colorArr[0], colorArr[1], colorArr[2]));
                }

                bool visible = selected->IsVisible();
                if (ImGui::Checkbox("Visible", &visible)) {
                    selected->SetVisible(visible);
                }
            }
        } else {
            ImGui::Text("No object selected");
        }
    }
    ImGui::End();
}

void EditorUI::DrawTransformControls(std::shared_ptr<GameObject> obj) {
    glm::vec3 pos = obj->GetPosition();
    glm::vec3 rot = obj->GetRotation();
    glm::vec3 scale = obj->GetScale();

    float posArr[3] = { pos.x, pos.y, pos.z };
    float rotArr[3] = { rot.x, rot.y, rot.z };
    float scaleArr[3] = { scale.x, scale.y, scale.z };

    if (ImGui::DragFloat3("Position", posArr, 0.1f)) {
        obj->SetPosition(glm::vec3(posArr[0], posArr[1], posArr[2]));
    }
    if (ImGui::DragFloat3("Rotation", rotArr, 1.0f, -180.0f, 180.0f)) {
        obj->SetRotation(glm::vec3(rotArr[0], rotArr[1], rotArr[2]));
    }
    if (ImGui::DragFloat3("Scale", scaleArr, 0.1f, 0.001f, 10.0f)) {
        obj->SetScale(glm::vec3(scaleArr[0], scaleArr[1], scaleArr[2]));
    }

    ImGui::SameLine();
    if (ImGui::Button("R##Pos")) obj->SetPosition(glm::vec3(0.0f));
    ImGui::SameLine();
    if (ImGui::Button("R##Rot")) obj->SetRotation(glm::vec3(0.0f));
    ImGui::SameLine();
    if (ImGui::Button("R##Scale")) obj->SetScale(glm::vec3(1.0f));
}

// ===== ИСПРАВЛЕННАЯ ФУНКЦИЯ SCENE VIEW =====
void EditorUI::DrawSceneView() {
    // ВАЖНО: Убираем возможность перетаскивания окна
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | 
                             ImGuiWindowFlags_NoScrollWithMouse |
                             ImGuiWindowFlags_NoMove;  // ← НЕ ДАЁТ ДВИГАТЬ ОКНО

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

    if (ImGui::Begin("Scene View", nullptr, flags)) {
        m_ViewportSize = ImGui::GetContentRegionAvail();
        m_ViewportPos = ImGui::GetCursorScreenPos();
        m_ViewportHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);
        m_ViewportFocused = ImGui::IsWindowFocused();

        // Рисуем фон
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 p0 = m_ViewportPos;
        ImVec2 p1 = ImVec2(p0.x + m_ViewportSize.x, p0.y + m_ViewportSize.y);

        drawList->AddRectFilled(p0, p1, IM_COL32(
            int(m_Settings.bg_color[0] * 255),
            int(m_Settings.bg_color[1] * 255),
            int(m_Settings.bg_color[2] * 255), 255));

        // Сетка
        if (m_Settings.grid_enabled) {
            float gridStep = 50.0f;
            for (float x = 0; x < m_ViewportSize.x; x += gridStep) {
                drawList->AddLine(ImVec2(p0.x + x, p0.y), ImVec2(p0.x + x, p1.y), IM_COL32(50, 50, 60, 100));
            }
            for (float y = 0; y < m_ViewportSize.y; y += gridStep) {
                drawList->AddLine(ImVec2(p0.x, p0.y + y), ImVec2(p1.x, p0.y + y), IM_COL32(50, 50, 60, 100));
            }
        }

        // ===== ImGuizmo ОТРИСОВКА =====
        auto selected = GetSelectedObject();
        if (selected && m_Settings.show_gizmo && m_ViewportHovered) {
            // 1. Включаем гизмо
            ImGuizmo::Enable(true);
            
            // 2. Устанавливаем контекст
            ImGuizmo::SetImGuiContext(m_ImGuiContext);
            
            // 3. Устанавливаем drawlist
            ImGuizmo::SetDrawlist();

            // 4. ВАЖНО: Устанавливаем прямоугольник viewport (координаты экрана!)
            ImGuizmo::SetRect(m_ViewportPos.x, m_ViewportPos.y, m_ViewportSize.x, m_ViewportSize.y);

            // 5. Получаем матрицу трансформации объекта
            glm::mat4 transform = selected->GetTransformMatrix();
            float* matrixPtr = glm::value_ptr(transform);

            // 6. Манипуляция матрицей через ImGuizmo
            if (ImGuizmo::Manipulate(
                glm::value_ptr(m_ViewMatrix),
                glm::value_ptr(m_ProjectionMatrix),
                m_CurrentGizmoOperation,
                m_CurrentGizmoMode,
                matrixPtr
            )) {
                // Разбираем обновлённую матрицу обратно в компоненты
                glm::vec3 newScale;
                glm::quat newRotation;
                glm::vec3 newPosition;
                glm::vec3 skew;
                glm::vec4 perspective;

                glm::decompose(glm::make_mat4(matrixPtr), newScale, newRotation, newPosition, skew, perspective);

                selected->SetPosition(newPosition);

                // Конвертируем квартернион в углы Эйлера (градусы)
                glm::vec3 newEuler = glm::degrees(glm::eulerAngles(newRotation));
                selected->SetRotation(newEuler);

                selected->SetScale(newScale);
            }
        }
        // ================================

        // Инфо-текст
        ImGui::SetCursorPos(ImVec2(10, 10));
        ImGui::TextColored(ImVec4(1, 1, 1, 0.7f), "Scene View");
        ImGui::SetCursorPos(ImVec2(10, 30));
        ImGui::TextColored(ImVec4(1, 1, 1, 0.5f), "%.0fx%.0f", m_ViewportSize.x, m_ViewportSize.y);

    }
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}

void EditorUI::DrawSceneSettings() {
    if (ImGui::Begin("Scene Settings")) {
        if (ImGui::CollapsingHeader("Lighting")) {
            ImGui::DragFloat3("Light Position", glm::value_ptr(m_Settings.light_pos), 0.1f);
            ImGui::ColorEdit3("Light Color", glm::value_ptr(m_Settings.light_color));
            ImGui::SliderFloat("Light Intensity", &m_Settings.light_intensity, 0.0f, 5.0f);
        }

        if (ImGui::CollapsingHeader("Environment")) {
            ImGui::ColorEdit3("Background", m_Settings.bg_color);
            ImGui::Checkbox("Show Grid", &m_Settings.grid_enabled);
            ImGui::Checkbox("Wireframe Mode", &m_Settings.wireframe_mode);
            ImGui::Checkbox("Show Gizmo", &m_Settings.show_gizmo);
        }

        ImGui::Separator();
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    }
    ImGui::End();
}

void EditorUI::DrawContentBrowser() {
    if (ImGui::Begin("Content Browser")) {
        ImGui::Text("Project Files");
        ImGui::Separator();
        ImGui::Text("Assets/");
    }
    ImGui::End();
}

void EditorUI::DrawMaterialSettings() {
    if (ImGui::Begin("Material Settings")) {
        ImGui::SliderFloat("Shininess", &m_Settings.shininess, 1.0f, 256.0f);
        ImGui::SliderFloat("Metallic", &m_Settings.metallic, 0.0f, 1.0f);
        ImGui::SliderFloat("Roughness", &m_Settings.roughness, 0.0f, 1.0f);
    }
    ImGui::End();
}

void EditorUI::HandleShortcuts() {
    ImGuiIO& io = ImGui::GetIO();

    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S, false)) {
        if (m_SceneManager) m_SceneManager->SaveScene("scene.binax");
    }

    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_D, false)) {
        if (m_SceneManager) m_SceneManager->DuplicateSelectedObject();
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Delete, false)) {
        if (m_SceneManager) {
            auto selected = m_SceneManager->GetSelectedObject();
            if (selected) m_SceneManager->DeleteGameObject(selected.get());
        }
    }

    // ===== ГОРЯЧИЕ КЛАВИШИ ImGuizmo =====
    if (!io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_W, false)) {
        m_CurrentGizmoOperation = ImGuizmo::TRANSLATE;
    }
    if (!io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_E, false)) {
        m_CurrentGizmoOperation = ImGuizmo::ROTATE;
    }
    if (!io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_R, false)) {
        m_CurrentGizmoOperation = ImGuizmo::SCALE;
    }
    if (!io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_X, false)) {
        m_CurrentGizmoMode = (m_CurrentGizmoMode == ImGuizmo::WORLD) ? ImGuizmo::LOCAL : ImGuizmo::WORLD;
    }
    // ===================================

    if (ImGui::IsKeyPressed(ImGuiKey_G, false)) {
        m_Settings.grid_enabled = !m_Settings.grid_enabled;
    }
}

std::shared_ptr<GameObject> EditorUI::GetSelectedObject() const {
    return m_SceneManager ? m_SceneManager->GetSelectedObject() : nullptr;
}

void EditorUI::SetSelectedObject(std::shared_ptr<GameObject> obj) {
    if (m_SceneManager) m_SceneManager->SetSelectedObject(obj);
}