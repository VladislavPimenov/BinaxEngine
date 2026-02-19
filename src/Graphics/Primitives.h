#pragma once
#include <memory>
#include "Graphics/Mesh.h"

class Primitives {
public:
    static std::shared_ptr<Mesh> CreateCube();
    static std::shared_ptr<Mesh> CreateGrid(int size);
    static std::shared_ptr<Mesh> CreateSphere(int segments = 32);
    static std::shared_ptr<Mesh> CreateCylinder(int segments = 32);
    static std::shared_ptr<Mesh> CreateCone(int segments = 32);
    static std::shared_ptr<Mesh> CreatePyramid();
    static std::shared_ptr<Mesh> CreatePlane();
};