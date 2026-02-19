cmake_minimum_required(VERSION 3.15)
project(BinaxEngine)
set(CMAKE_CXX_STANDARD 17)

# ========== ПУТИ ==========
set(LIBS_DIR "${CMAKE_SOURCE_DIR}/libs")
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
set(SHADERS_DIR "${ASSETS_DIR}/shaders")

# GLFW
set(GLFW_INCLUDE "${LIBS_DIR}/glfw/include")
set(GLFW_LIB "${LIBS_DIR}/glfw/lib-vc2022/glfw3.lib")

# GLEW
set(GLEW_INCLUDE "${LIBS_DIR}/glew/include")
set(GLEW_LIB "${LIBS_DIR}/glew/lib/Release/x64/glew32.lib")

# ImGui
set(IMGUI_DIR "${LIBS_DIR}/imgui")

# GLM
set(GLM_DIR "${LIBS_DIR}/glm")

# ========== ПРОВЕРКИ ==========
if(NOT EXISTS "${GLFW_INCLUDE}/GLFW/glfw3.h")
    message(FATAL_ERROR "GLFW header not found at: ${GLFW_INCLUDE}")
endif()

if(NOT EXISTS "${GLEW_INCLUDE}/GL/glew.h")
    message(FATAL_ERROR "GLEW header not found at: ${GLEW_INCLUDE}")
endif()

if(NOT EXISTS "${IMGUI_DIR}/imgui.cpp")
    message(FATAL_ERROR "ImGui not found at: ${IMGUI_DIR}")
endif()

if(NOT EXISTS "${GLM_DIR}/glm/glm.hpp")
    message(FATAL_ERROR "GLM not found at: ${GLM_DIR}")
endif()

# Проверка папки с шейдерами
if(NOT EXISTS "${SHADERS_DIR}")
    message(WARNING "Shaders directory not found at: ${SHADERS_DIR}")
    file(MAKE_DIRECTORY "${SHADERS_DIR}")
endif()

message(STATUS "=== Libraries found ===")
message(STATUS "GLFW: ${GLFW_INCLUDE}")
message(STATUS "GLEW: ${GLEW_INCLUDE}")
message(STATUS "ImGui: ${IMGUI_DIR}")
message(STATUS "GLM: ${GLM_DIR}")
message(STATUS "Shaders: ${SHADERS_DIR}")

# ========== ИСХОДНЫЕ ФАЙЛЫ ==========
set(SOURCES
    src/main.cpp
    src/Graphics/Shader.cpp
    src/Graphics/Mesh.cpp
    src/Graphics/Primitives.cpp
    src/Scene/GameObject.cpp
    src/Scene/SceneManager.cpp
    src/Editor/EditorUI.cpp
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    # ===== ImGuizmo =====
    ${IMGUI_DIR}/ImGuizmo.h
    ${IMGUI_DIR}/ImGuizmo.cpp
)

# ========== ИСПОЛНЯЕМЫЙ ФАЙЛ ==========
add_executable(BinaxEngine ${SOURCES})

# ========== ВКЛЮЧАЕМ ДИРЕКТОРИИ ==========
target_include_directories(BinaxEngine PRIVATE
    src
    src/Editor
    ${GLFW_INCLUDE}
    ${GLEW_INCLUDE}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${GLM_DIR}
    ${ASSETS_DIR}
)

# ========== ОПРЕДЕЛЯЕМ ПУТЬ К ASSETS ДЛЯ КОДА ==========
target_compile_definitions(BinaxEngine PRIVATE
    ASSETS_DIR="${ASSETS_DIR}"
)

# ========== ПОДКЛЮЧАЕМ БИБЛИОТЕКИ ==========
target_link_libraries(BinaxEngine
    ${GLFW_LIB}
    ${GLEW_LIB}
    opengl32.lib
)

# ========== КОПИРОВАНИЕ ФАЙЛОВ ПОСЛЕ СБОРКИ ==========
if(WIN32)
    # Копирование DLL
    set(GLFW_DLL "${LIBS_DIR}/glfw/lib-vc2022/glfw3.dll")
    if(EXISTS "${GLFW_DLL}")
        add_custom_command(TARGET BinaxEngine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${GLFW_DLL}"
            $<TARGET_FILE_DIR:BinaxEngine>
        )
        message(STATUS "GLFW DLL will be copied")
    endif()

    set(GLEW_DLL "${LIBS_DIR}/glew/bin/Release/x64/glew32.dll")
    if(EXISTS "${GLEW_DLL}")
        add_custom_command(TARGET BinaxEngine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${GLEW_DLL}"
            $<TARGET_FILE_DIR:BinaxEngine>
        )
        message(STATUS "GLEW DLL will be copied")
    endif()

    # Копирование шейдеров
    if(EXISTS "${SHADERS_DIR}")
        add_custom_command(TARGET BinaxEngine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${SHADERS_DIR}"
            "$<TARGET_FILE_DIR:BinaxEngine>/assets/shaders"
            COMMENT "Copying shaders to output directory"
        )
        message(STATUS "Shaders will be copied to: $<TARGET_FILE_DIR:BinaxEngine>/assets/shaders")
    endif()
endif()

message(STATUS "=== Binax Engine Configuration Complete ===")