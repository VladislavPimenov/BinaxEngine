#pragma once
#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Graphics/Shader.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"   
#include <glm/gtc/quaternion.hpp>

// Forward declarations for Bullet (чтобы компилятор знал, что это классы)
class btRigidBody;
class btCollisionShape;

enum LightType {
    LT_NONE = -1,
    LT_DIRECTIONAL = 0,
    LT_POINT = 1,
    LT_SPOT = 2
};

enum ColliderType {
    COLLIDER_NONE,
    COLLIDER_BOX,
    COLLIDER_SPHERE,
    COLLIDER_CAPSULE
};

class GameObject {
public:
    GameObject(const std::string& name = "GameObject");
    ~GameObject();

    // Иерархия
    void AddChild(std::shared_ptr<GameObject> child);
    void RemoveChild(GameObject* child);
    GameObject* GetParent() const { return m_Parent; }
    const std::vector<std::shared_ptr<GameObject>>& GetChildren() const { return m_Children; }

    // Трансформация
    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::vec3& rotation);
    void SetScale(const glm::vec3& scale);
    glm::vec3 GetPosition() const { return m_Position; }
    glm::vec3 GetRotation() const { return m_Rotation; }
    glm::vec3 GetScale() const { return m_Scale; }
    glm::vec3 GetWorldPosition() const;
    glm::mat4 GetTransformMatrix() const;

    // Меш и видимость
    void SetMesh(std::shared_ptr<Mesh> mesh) { m_Mesh = mesh; }
    std::shared_ptr<Mesh> GetMesh() const { return m_Mesh; }
    void SetVisible(bool visible) { m_Visible = visible; }
    bool IsVisible() const { return m_Visible; }

    // Имя и цвет
    void SetName(const std::string& name) { m_Name = name; }
    std::string GetName() const { return m_Name; }
    void SetColor(const glm::vec3& color) { m_Color = color; }
    glm::vec3 GetColor() const { return m_Color; }

    // ===== Материал =====
    void SetMaterial(std::shared_ptr<Material> mat) { m_Material = mat; }
    std::shared_ptr<Material> GetMaterial() const { return m_Material; }

    // Отрисовка
    void Draw(Shader& shader) const;
    
    void SetCastShadows(bool cast) { m_CastShadows = cast; }
    bool CastShadows() const { return m_CastShadows; }
    void SetReceiveShadows(bool receive) { m_ReceiveShadows = receive; }
    bool ReceiveShadows() const { return m_ReceiveShadows; }

    void SetLightType(int type) { m_LightType = type; }
    int GetLightType() const { return m_LightType; }
    void SetLightColor(const glm::vec3& color) { m_LightColor = color; }
    glm::vec3 GetLightColor() const { return m_LightColor; }
    void SetLightIntensity(float intensity) { m_LightIntensity = intensity; }
    float GetLightIntensity() const { return m_LightIntensity; }
    void SetLightRange(float range) { m_LightRange = range; }
    float GetLightRange() const { return m_LightRange; }
    void SetLightAngle(float angleDeg) { m_LightAngle = glm::radians(angleDeg); }
    float GetLightAngleDeg() const { return glm::degrees(m_LightAngle); }
    void SetLightDirection(const glm::vec3& dir) { m_LightDirection = glm::normalize(dir); }
    glm::vec3 GetLightDirection() const { return m_LightDirection; }

     bool IsCamera() const { return m_IsCamera; }
    void SetIsCamera(bool isCamera) { m_IsCamera = isCamera; }
    
    float GetCameraFOV() const { return m_CameraFOV; }
    void SetCameraFOV(float fov) { m_CameraFOV = fov; }
    
    float GetCameraNear() const { return m_CameraNear; }
    void SetCameraNear(float nearVal) { m_CameraNear = nearVal; }
    
    float GetCameraFar() const { return m_CameraFar; }
    void SetCameraFar(float farVal) { m_CameraFar = farVal; }
    
    // Получить матрицы вида и проекции от этой камеры
    glm::mat4 GetCameraViewMatrix() const;
    glm::mat4 GetCameraProjectionMatrix(float aspectRatio) const;

     // Физика
    void AddRigidBody(float mass = 1.0f);
    void RemoveRigidBody();
    bool HasRigidBody() const { return m_rigidBody != nullptr; }
    float GetMass() const { return m_mass; }
    void SetMass(float mass) { m_mass = mass; }

    void SetColliderType(ColliderType type);
    ColliderType GetColliderType() const { return m_colliderType; }

    // Синхронизация трансформации с физическим телом
    void SyncTransformToPhysics();
    void SyncPhysicsToTransform();

    // Сброс в начальное состояние (для кнопки Return)
    void SaveInitialTransform();
    void ResetToInitialTransform();

    // Параметры материала
   void SetFriction(float friction);
   float GetFriction() const { return m_friction; }
   void SetRestitution(float restitution);
   float GetRestitution() const { return m_restitution; }
   void SetRollingFriction(float rollingFriction);
   float GetRollingFriction() const { return m_rollingFriction; }

// Демпфирование
    void SetLinearDamping(float damping);
    float GetLinearDamping() const { return m_linearDamping; }
    void SetAngularDamping(float damping);
    float GetAngularDamping() const { return m_angularDamping; } 

    // Доступ к btRigidBody
    btRigidBody* GetRigidBody() { return m_rigidBody; }

    void UpdatePhysicsBody(); // создаёт или обновляет физическое тело на основе коллайдера и массы


private:
    std::string m_Name;
    GameObject* m_Parent = nullptr;
    std::vector<std::shared_ptr<GameObject>> m_Children;

    glm::vec3 m_Position = glm::vec3(0.0f);
    glm::vec3 m_Rotation = glm::vec3(0.0f);
    glm::vec3 m_Scale = glm::vec3(1.0f);
    std::shared_ptr<Mesh> m_Mesh;
    glm::vec3 m_Color = glm::vec3(1.0f);
    bool m_Visible = true;
    bool m_CastShadows = true;
    bool m_ReceiveShadows = true;

    std::shared_ptr<Material> m_Material;   // <-- новое поле

    glm::vec3 m_PreviousRotation = glm::vec3(0.0f);

    int m_LightType = LT_NONE;
    glm::vec3 m_LightColor = glm::vec3(1.0f);
    float m_LightIntensity = 1.0f;
    float m_LightRange = 10.0f;
    float m_LightAngle = glm::radians(45.0f);
    glm::vec3 m_LightDirection = glm::vec3(0.0f, -1.0f, 0.0f);

    bool m_IsCamera = false;
    float m_CameraFOV = 45.0f;
    float m_CameraNear = 0.1f;
    float m_CameraFar = 100.0f;

    // Физические компоненты
    btRigidBody* m_rigidBody = nullptr;
    btCollisionShape* m_collisionShape = nullptr;
    ColliderType m_colliderType = COLLIDER_NONE;
    float m_mass = 0.0f;

    // Физические параметры материала
    float m_friction = 0.5f;
    float m_restitution = 0.5f;
    float m_rollingFriction = 0.1f;

// Демпфирование
    float m_linearDamping = 0.0f;
    float m_angularDamping = 0.0f;

    // Начальные трансформации для сброса
    glm::vec3 m_initialPosition;
    glm::vec3 m_initialRotation;
    glm::vec3 m_initialScale;
};
