# 🎮 BinaxEngine - Game Engine (v0.1.0 Alpha)

[![CMake](https://img.shields.io/badge/CMake-3.15+-blue?style=flat-square&logo=cmake)](https://cmake.org/)
[![C++](https://img.shields.io/badge/C++-17-orange?style=flat-square&logo=cplusplus)](https://isocpp.org/)
[![OpenGL](https://img.shields.io/badge/OpenGL-4.6-red?style=flat-square&logo=opengl)](https://www.opengl.org/)
[![Windows](https://img.shields.io/badge/Platform-Windows-blue?style=flat-square&logo=windows)](https://www.microsoft.com/windows)
[![Status](https://img.shields.io/badge/status-alpha-yellow?style=flat-square)]()

**⚠️ TEST VERSION - FIRST RELEASE ⚠️**  
This is the very first test release of BinaxEngine. It's experimental and under active development!

**BinaxEngine** is a lightweight game engine built with C++ and OpenGL, featuring an integrated editor for scene management and game development.

---

## ✨ Features (Current)

- 🔧 **Basic ImGui Editor Interface** — Simple editor with controls
- 📦 **Scene Management** — Create game objects
- 🎨 **Mesh & Primitive Rendering** — Basic shapes
- 🎥 **Shader System** — Custom shader support
- 🔍 **ImGuizmo Integration** — Transform controls (translate, rotate, scale)
- 📁 **Asset Management** — Organized assets directory structure
- 🚀 **CMake Build System** — Easy configuration

## 🔮 Planned Features

- [ ] Physics engine
- [ ] Audio system
- [ ] Particle systems
- [ ] Scene serialization
- [ ] Material system
- [ ] Lighting
- [ ] Terrain generation
- [ ] Scripting support

---

## 🛠️ Dependencies

The engine uses the following libraries (included in `libs/`):

- **[GLFW](https://www.glfw.org/)** — Window creation and input (v3.3)
- **[GLEW](https://glew.sourceforge.net/)** — OpenGL extension wrangler (v2.2)
- **[ImGui](https://github.com/ocornut/imgui)** — Immediate mode GUI (v1.90)
- **[ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)** — 3D manipulation controls
- **[GLM](https://github.com/g-truc/glm)** — Mathematics for graphics (v0.9.9)

---
## 🚀 Getting Started

### Prerequisites

- **CMake** 3.15 or higher
- **Visual Studio 2022** (with C++ development tools)
- **Windows 10/11** (currently Windows-only)

### Building from Source

1. **Clone the repository**
   ```bash
   git clone https://github.com/YOUR_USERNAME/BinaxEngine.git
   cd BinaxEngine
  
### Build with CMake

mkdir build
cd build
cmake ..
cmake --build . --config Release

### Run

./Release/BinaxEngine.exe
