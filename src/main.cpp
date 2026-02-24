#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Graphics/Shader.h"
#include "Graphics/Skybox.h"          // добавили
#include "Graphics/Primitives.h"
#include "Scene/SceneManager.h"
#include "Scene/Camera.h"
#include "Editor/EditorUI.h"

// Размеры окна
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

// Глобальные объекты
SceneManager g_SceneManager;
EditorUI g_EditorUI;
Camera g_Camera(glm::vec3(0.0f, 2.0f, 5.0f));

// Тайминги
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Мышь
bool firstMouse = true;
bool mouseCaptured = false;

// Шейдеры
Shader shader;
Shader gridShader;
Shader gizmoShader;
Shader skyboxShader;          // <-- добавили

// Skybox
Skybox skybox;

// Тени (пока не используются)
unsigned int depthMapFBO = 0;
unsigned int depthMap = 0;
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

// Прототипы
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
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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

    // 8. Загрузка Skybox (после шейдеров)
    if (!skybox.Load(
        "assets/textures/skybox/right.png",
        "assets/textures/skybox/left.png",
        "assets/textures/skybox/top.png",
        "assets/textures/skybox/bottom.png",
        "assets/textures/skybox/front.png",
        "assets/textures/skybox/back.png"
    )) {
        std::cerr << "Failed to load skybox textures" << std::endl;
    }

    // 9. Инициализация shadow map
    initShadowMap();

    // 10. Главный цикл
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        g_EditorUI.HandleShortcuts();

        auto& settings = g_EditorUI.GetSettings();
        glm::vec3 lightPos = settings.light_pos;
        glm::vec3 lightColor = settings.light_color;

        // Матрицы проекции и вида
        float aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
        glm::mat4 projection = g_Camera.GetProjectionMatrix(aspect);
        glm::mat4 view = g_Camera.GetViewMatrix();

        // Очистка буфера
        glClearColor(settings.bg_color[0], settings.bg_color[1], settings.bg_color[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ========== РЕНДЕР SKYBOX ==========
        glDepthMask(GL_FALSE); // отключаем запись в буфер глубины
        skyboxShader.Use();
        // Убираем перемещение из матрицы вида (оставляем только поворот)
        glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view));
        skyboxShader.SetMat4("view", glm::value_ptr(viewNoTranslation));
        skyboxShader.SetMat4("projection", glm::value_ptr(projection));
        skybox.Draw();
        glDepthMask(GL_TRUE);
        // ====================================

        // Полигональный режим для сцены
        if (settings.wireframe_mode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Использование основного шейдера
        shader.Use();
        shader.SetMat4("projection", glm::value_ptr(projection));
        shader.SetMat4("view", glm::value_ptr(view));
        shader.SetVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
        shader.SetVec3("lightColor", lightColor.x, lightColor.y, lightColor.z);
        shader.SetFloat("lightIntensity", settings.light_intensity);
        shader.SetVec3("viewPos", g_Camera.GetPosition().x, g_Camera.GetPosition().y, g_Camera.GetPosition().z);
        shader.SetFloat("shininess", settings.shininess);
        shader.SetFloat("metallic", settings.metallic);
        shader.SetFloat("roughness", settings.roughness);
        shader.SetBool("shadowsEnabled", false);

        // Рендер сцены
        g_SceneManager.Render(shader);

        // Возвращаем заполненный режим
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // ===== ПЕРЕДАЧА МАТРИЦ В UI =====
        g_EditorUI.SetViewProjection(view, projection);

        // Отрисовка UI
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
    bool skyboxLoaded = skyboxShader.Load("assets/shaders/skybox.vert", "assets/shaders/skybox.frag"); // <-- добавили

    if (!shadersLoaded || !gridLoaded || !gizmoLoaded || !skyboxLoaded) {
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
    float speed = 5.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        g_Camera.MoveForward(speed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        g_Camera.MoveBackward(speed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        g_Camera.MoveLeft(speed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        g_Camera.MoveRight(speed);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        g_Camera.MoveUp(speed);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        g_Camera.MoveDown(speed);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (!mouseCaptured) return;
    static float lastX = SCR_WIDTH / 2.0f;
    static float lastY = SCR_HEIGHT / 2.0f;
    static bool firstMouse = true;
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    g_Camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    g_Camera.ProcessMouseScroll(static_cast<float>(yoffset));
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