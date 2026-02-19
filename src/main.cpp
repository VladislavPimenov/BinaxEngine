#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Graphics/Shader.h"
#include "Graphics/Primitives.h"
#include "Scene/SceneManager.h"
#include "Editor/EditorUI.h"

// Размеры окна
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

// Камера
glm::vec3 cameraPos = glm::vec3(0.0f, 2.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, -0.2f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Тайминги
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Мышь
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float yaw = -90.0f;
float pitch = 0.0f;
bool firstMouse = true;
bool mouseCaptured = false;

// Параметры камеры
float cameraSpeed = 2.5f;
float fov = 45.0f;

// Менеджер сцены и UI
SceneManager g_SceneManager;
EditorUI g_EditorUI;

// Шейдеры
Shader shader;
Shader gridShader;
Shader gizmoShader;

// Shadow mapping
unsigned int depthMapFBO = 0;
unsigned int depthMap = 0;
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

// Прототипы функций
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
bool initShaders();
bool initShadowMap();

int main() {
    std::cout << "=== Binax Engine Editor ===" << std::endl;

    // 1. Инициализация GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    
    // ←←← ←←← ←←← ВАЖНО: ИЗМЕНИЛИ CORE НА COMPAT ДЛЯ ГИЗМО ←←← ←←← ←←←
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // ← ЗАКОММЕНТИРОВАНО
    // ←←← ←←← ←←← ВАЖНО: ИЗМЕНИЛИ CORE НА COMPAT ДЛЯ ГИЗМО ←←← ←←← ←←←
    
    glfwWindowHint(GLFW_SAMPLES, 4);

    // 2. Создание окна
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Binax Engine Editor", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    mouseCaptured = false;

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    // 3. Инициализация GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW init failed!" << std::endl;
        return -1;
    }

    // 4. Настройка OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GPU: " << glGetString(GL_RENDERER) << std::endl;

    // 5. Инициализация сцены
    g_SceneManager.Initialize();

    // 6. Инициализация UI
    if (!g_EditorUI.Initialize(window, &g_SceneManager)) {
        std::cerr << "Failed to initialize EditorUI" << std::endl;
        return -1;
    }

    // 7. Загрузка шейдеров
    if (!initShaders()) {
        return -1;
    }

    // 8. Инициализация shadow map
    initShadowMap();

    // 9. Главный цикл
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        g_EditorUI.HandleShortcuts();

        auto& settings = g_EditorUI.GetSettings();
        glm::vec3 lightPos = settings.light_pos;
        glm::vec3 lightColor = settings.light_color;

        glm::mat4 projection = glm::perspective(glm::radians(fov), 
                                                (float)SCR_WIDTH / (float)SCR_HEIGHT, 
                                                0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glClearColor(settings.bg_color[0], settings.bg_color[1], settings.bg_color[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (settings.wireframe_mode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        shader.Use();
        shader.SetMat4("projection", glm::value_ptr(projection));
        shader.SetMat4("view", glm::value_ptr(view));
        shader.SetVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
        shader.SetVec3("lightColor", lightColor.x, lightColor.y, lightColor.z);
        shader.SetFloat("lightIntensity", settings.light_intensity);
        shader.SetVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
        shader.SetFloat("shininess", settings.shininess);
        shader.SetFloat("metallic", settings.metallic);
        shader.SetFloat("roughness", settings.roughness);

        if (settings.shadows_enabled && depthMap != 0) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            shader.SetInt("shadowMap", 1);
            shader.SetBool("shadowsEnabled", true);
            shader.SetFloat("shadowBias", settings.shadow_bias);
        } else {
            shader.SetBool("shadowsEnabled", false);
        }

        // Рендерим сцену
        g_SceneManager.Render(shader);

        // ===== Рендерим гизмо =====
        if (settings.show_gizmo) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            g_SceneManager.RenderGizmos(shader);
        }
        // =========================

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // ===== ПЕРЕДАЧА МАТРИЦ В UI ДЛЯ ImGuizmo =====
        g_EditorUI.SetViewProjection(view, projection);
// =============================================

        g_EditorUI.BeginFrame();
        g_EditorUI.Render();
        g_EditorUI.EndFrame();          

        g_EditorUI.BeginFrame();
        g_EditorUI.Render();
        g_EditorUI.EndFrame();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    g_EditorUI.Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "Binax Engine shutdown successfully." << std::endl;
    return 0;
}

bool initShaders() {
    std::cout << "Loading shaders from assets/shaders/..." << std::endl;
    bool shadersLoaded = shader.Load("assets/shaders/basic.vert", "assets/shaders/basic.frag");
    bool gridLoaded = gridShader.Load("assets/shaders/grid.vert", "assets/shaders/grid.frag");
    bool gizmoLoaded = gizmoShader.Load("assets/shaders/gizmo.vert", "assets/shaders/gizmo.frag");
    
    if (!shadersLoaded || !gridLoaded || !gizmoLoaded) {
        std::cerr << "ERROR: Failed to load shaders!" << std::endl;
        return false;
    }
    std::cout << "All shaders loaded successfully!" << std::endl;
    return true;
}

bool initShadowMap() {
    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer not complete!" << std::endl;
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void processInput(GLFWwindow* window) {
    if (!mouseCaptured) return;
    
    float speed = cameraSpeed * deltaTime;
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += speed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= speed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos += speed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraPos -= speed * cameraUp;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (!mouseCaptured) return;
    
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    fov -= (float)yoffset;
    if (fov < 20.0f) fov = 20.0f;
    if (fov > 90.0f) fov = 90.0f;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        mouseCaptured = false;
        firstMouse = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    
    if (key == GLFW_KEY_DELETE && action == GLFW_PRESS) {
        auto selected = g_SceneManager.GetSelectedObject();
        if (selected)
            g_SceneManager.DeleteGameObject(selected.get());
    }
}