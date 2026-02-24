# 🎮 BinaxEngine - Game Engine (v0.2.0 Alpha)

[![CMake](https://img.shields.io/badge/CMake-3.15+-blue?style=flat-square&logo=cmake)](https://cmake.org/)
[![C++](https://img.shields.io/badge/C++-17-orange?style=flat-square&logo=cplusplus)](https://isocpp.org/)
[![OpenGL](https://img.shields.io/badge/OpenGL-4.6-red?style=flat-square&logo=opengl)](https://www.opengl.org/)
[![Windows](https://img.shields.io/badge/Platform-Windows-blue?style=flat-square&logo=windows)](https://www.microsoft.com/windows)
[![Status](https://img.shields.io/badge/status-alpha-yellow?style=flat-square)]()

**⚠️ TEST VERSION - EXPERIMENTAL RELEASE ⚠️**  
This is a test release of BinaxEngine. Everything is work in progress, bugs are expected, and the engine is constantly evolving!

**BinaxEngine** is a lightweight game engine built with C++ and OpenGL, featuring an integrated editor for scene management and game development.

---

## ✨ Features (Current)

- 🔧 **ImGui Editor Interface** — Intuitive editor with docking and controls
- 📦 **Scene Management** — Create, delete, and duplicate game objects
- 🎨 **Mesh & Primitive Rendering** — Cubes, spheres, grids, pyramids
- 🎥 **Shader System** — Custom shader support with live reload (coming soon)
- 🔍 **ImGuizmo Integration** — 3D transform controls (translate, rotate, scale)
- 📁 **Asset Management** — Organized assets directory with shader and texture support

### 🆕 New in v0.2.0
- 🖼️ **Material System** — Per-object materials with adjustable parameters
- 📥 **Texture Loading** — Load diffuse and normal maps via file dialog
- 🌄 **Normal Mapping** — Realistic surface detail with normal maps and adjustable strength
- 🔄 **UV Control** — Per-material UV scaling
- 🌌 **Skybox** — Cubemap-based environment background
- ⚙️ **Material Parameters** — Metallic and roughness sliders for PBR-style workflow
- 🎛️ **Real-time Material Inspector** — Edit material properties on the fly
- 🌞 **Lighting Controls** — Adjustable light position, color, and intensity

## 🔮 Planned Features

- [ ] Physics engine (Bullet or custom)
- [ ] Audio system (OpenAL or SoLoud)
- [ ] Particle systems
- [ ] Scene serialization (save/load)
- [ ] Multiple light sources
- [ ] Terrain generation
- [ ] Scripting support (Lua)
- [ ] Shadow mapping
- [ ] Post-processing (bloom, HDR)

---

## 🛠️ Dependencies

The engine uses the following libraries (included in `libs/`):

- **[GLFW](https://www.glfw.org/)** — Window creation and input (v3.3)
- **[GLEW](https://glew.sourceforge.net/)** — OpenGL extension wrangler (v2.2)
- **[ImGui](https://github.com/ocornut/imgui)** — Immediate mode GUI (v1.90)
- **[ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)** — 3D manipulation controls
- **[GLM](https://github.com/g-truc/glm)** — Mathematics for graphics (v0.9.9)
- **[stb_image](https://github.com/nothings/stb)** — Image loading

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
