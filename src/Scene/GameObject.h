#pragma once
#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Graphics/Shader.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"   
#include <glm/gtc/quaternion.hpp>

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

class GameObject : public std::enable_shared_from_this<GameObject> {
public:
    GameObject(const std::string& name = "GameObject");
    ~GameObject();

    // Иерархия
    void AddChild(std::shared_ptr<GameObject> child);
    void RemoveChild(GameObject* child);
    GameObject* GetParent() const { return m_Parent; }
    const std::vector<std::shared_ptr<GameObject>>& GetChildren() const { return m_Children; }

    // Проверка, можно ли добавлять физику (нет родителей и детей)
    bool CanHavePhysics() const;

    // Управление иерархией
    void Unparent();  // открепить от родителя, сохранив мировую трансформацию
    void SetParent(std::shared_ptr<GameObject> newParent, bool keepWorldPosition = true);

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

    // Материал
    void SetMaterial(std::shared_ptr<Material> mat) { m_Material = mat; }
    std::shared_ptr<Material> GetMaterial() const { return m_Material; }

    // Отрисовка
    void Draw(Shader& shader) const;
    
    void SetCastShadows(bool cast) { m_CastShadows = cast; }
    bool CastShadows() const { return m_CastShadows; }
    void SetReceiveShadows(bool receive) { m_ReceiveShadows = receive; }
    bool ReceiveShadows() const { return m_ReceiveShadows; }

    // Light
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

    // Camera
    bool IsCamera() const { return m_IsCamera; }
    void SetIsCamera(bool isCamera) { m_IsCamera = isCamera; }
    float GetCameraFOV() const { return m_CameraFOV; }
    void SetCameraFOV(float fov) { m_CameraFOV = fov; }
    float GetCameraNear() const { return m_CameraNear; }
    void SetCameraNear(float nearVal) { m_CameraNear = nearVal; }
    float GetCameraFar() const { return m_CameraFar; }
    void SetCameraFar(float farVal) { m_CameraFar = farVal; }
    glm::mat4 GetCameraViewMatrix() const;
    glm::mat4 GetCameraProjectionMatrix(float aspectRatio) const;

    // Physics
    void AddRigidBody(float mass = 1.0f);
    void RemoveRigidBody();
    bool HasRigidBody() const { return m_rigidBody != nullptr; }
    float GetMass() const { return m_mass; }
    void SetMass(float mass) { m_mass = mass; }
    void SetColliderType(ColliderType type);
    ColliderType GetColliderType() const { return m_colliderType; }
    void SyncTransformToPhysics();
    void SyncPhysicsToTransform();
    void SaveInitialTransform();
    void ResetToInitialTransform();
    void SetFriction(float friction);
    float GetFriction() const { return m_friction; }
    void SetRestitution(float restitution);
    float GetRestitution() const { return m_restitution; }
    void SetRollingFriction(float rollingFriction);
    float GetRollingFriction() const { return m_rollingFriction; }
    void SetLinearDamping(float damping);
    float GetLinearDamping() const { return m_linearDamping; }
    void SetAngularDamping(float damping);
    float GetAngularDamping() const { return m_angularDamping; }
    btRigidBody* GetRigidBody() { return m_rigidBody; }
    void UpdatePhysicsBody();

    // В public секцию
    void SetIsFog(bool fog) { m_IsFog = fog; }
    bool IsFog() const { return m_IsFog; }

    void SetFogEnabled(bool enabled) { m_FogEnabled = enabled; }
    bool GetFogEnabled() const { return m_FogEnabled; }
    void SetFogType(int type) { m_FogType = type; }
    int GetFogType() const { return m_FogType; }
    void SetFogColor(const glm::vec3& color) { m_FogColor = color; }
    glm::vec3 GetFogColor() const { return m_FogColor; }
    void SetFogDensity(float density) { m_FogDensity = density; }
    float GetFogDensity() const { return m_FogDensity; }
    void SetFogLinearStart(float start) { m_FogLinearStart = start; }
    float GetFogLinearStart() const { return m_FogLinearStart; }
    void SetFogLinearEnd(float end) { m_FogLinearEnd = end; }
    float GetFogLinearEnd() const { return m_FogLinearEnd; }

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
    std::shared_ptr<Material> m_Material;
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

    btRigidBody* m_rigidBody = nullptr;
    btCollisionShape* m_collisionShape = nullptr;
    ColliderType m_colliderType = COLLIDER_NONE;
    float m_mass = 0.0f;

    float m_friction = 0.5f;
    float m_restitution = 0.5f;
    float m_rollingFriction = 0.1f;
    float m_linearDamping = 0.0f;
    float m_angularDamping = 0.0f;

    // В private секцию (в конец списка полей)
    bool m_IsFog = false;
    bool m_FogEnabled = false;
    int m_FogType = 1;          // 1=Linear, 2=Exponential, 3=Exponential Squared
    glm::vec3 m_FogColor = glm::vec3(0.5f, 0.6f, 0.7f);
    float m_FogDensity = 0.04f;
    float m_FogLinearStart = 10.0f;
    float m_FogLinearEnd = 50.0f;

    glm::vec3 m_initialPosition;
    glm::vec3 m_initialRotation;
    glm::vec3 m_initialScale;
};
