#include "Scene/GameObject.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Physics/PhysicsWorld.h"
#include <btBulletDynamicsCommon.h>

GameObject::GameObject(const std::string& name)
    : m_Name(name) {
}

GameObject::~GameObject() {
    for (auto& child : m_Children) {
        child->m_Parent = nullptr;
    }
}

void GameObject::AddChild(std::shared_ptr<GameObject> child) {
    if (child->m_Parent == this) return;
    if (child->m_Parent) {
        child->m_Parent->RemoveChild(child.get());
    }
    child->m_Parent = this;
    m_Children.push_back(child);
}

void GameObject::RemoveChild(GameObject* child) {
    for (auto it = m_Children.begin(); it != m_Children.end(); ++it) {
        if (it->get() == child) {
            child->m_Parent = nullptr;
            m_Children.erase(it);
            break;
        }
    }
}

glm::vec3 GameObject::GetWorldPosition() const {
    if (m_Parent) {
        return m_Parent->GetWorldPosition() + m_Position;
    }
    return m_Position;
}

glm::mat4 GameObject::GetTransformMatrix() const {
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, m_Position);
    glm::mat4 rotation = glm::eulerAngleYXZ(
        glm::radians(m_Rotation.y),
        glm::radians(m_Rotation.x),
        glm::radians(m_Rotation.z)
    );
    transform = transform * rotation;
    transform = glm::scale(transform, m_Scale);
    if (m_Parent) {
        transform = m_Parent->GetTransformMatrix() * transform;
    }
    return transform;
}

void GameObject::SetPosition(const glm::vec3& position) {
    m_Position = position;
}

void GameObject::SetRotation(const glm::vec3& rotation) {
    m_PreviousRotation = m_Rotation;
    m_Rotation = rotation;
    auto normalize = [](float angle) {
        while (angle > 180.0f) angle -= 360.0f;
        while (angle < -180.0f) angle += 360.0f;
        return angle;
    };
    m_Rotation.x = normalize(m_Rotation.x);
    m_Rotation.y = normalize(m_Rotation.y);
    m_Rotation.z = normalize(m_Rotation.z);
}

void GameObject::SetScale(const glm::vec3& scale) {
    m_Scale = scale;
    if (m_Scale.x == 0.0f) m_Scale.x = 0.001f;
    if (m_Scale.y == 0.0f) m_Scale.y = 0.001f;
    if (m_Scale.z == 0.0f) m_Scale.z = 0.001f;
    
    // Если есть коллайдер, пересоздаём его с новым масштабом
    if (m_colliderType != COLLIDER_NONE) {
        SetColliderType(m_colliderType);  // пересоздаст коллайдер и вызовет UpdatePhysicsBody
    }
}

void GameObject::Draw(Shader& shader) const {
    if (!m_Visible || !m_Mesh) return;

    glm::mat4 transform = GetTransformMatrix();
    shader.SetMat4("model", glm::value_ptr(transform));
    shader.SetBool("receiveShadows", m_ReceiveShadows);
    shader.SetVec3("objectColor", m_Color.x, m_Color.y, m_Color.z);

    // Выбираем материал: сначала свой, потом из меша
    std::shared_ptr<Material> mat = m_Material;
    if (!mat && m_Mesh) {
        mat = m_Mesh->GetMaterial();
    }

    if (mat) {
        shader.SetBool("hasDiffuseTexture", mat->HasDiffuse());
        shader.SetBool("hasNormalMap", mat->HasNormal());
        shader.SetFloat("metallic", mat->metallic);
        shader.SetFloat("roughness", mat->roughness);
        shader.SetVec2("uvScale", mat->uvScale.x, mat->uvScale.y);
        shader.SetFloat("normalStrength", mat->normalStrength);
        shader.SetBool("useWorldUV", mat->useWorldUV);
        shader.SetVec3("emissionColor", mat->emissionColor.x, mat->emissionColor.y, mat->emissionColor.z);
        shader.SetFloat("emissionIntensity", mat->emissionIntensity);

        mat->BindTextures();
        shader.SetInt("diffuseTexture", 0);
        shader.SetInt("normalMap", 1);
        shader.SetBool("hasRoughnessTexture", mat->HasRoughness());
        shader.SetBool("hasMetallicTexture", mat->HasMetallic());
        shader.SetBool("hasAOTexture", mat->HasAO());
        shader.SetInt("roughnessTexture", 3);
        shader.SetInt("metallicTexture", 4);
        shader.SetInt("aoTexture", 5);
    } else {
        shader.SetBool("hasDiffuseTexture", false);
        shader.SetBool("hasNormalMap", false);
        shader.SetVec3("emissionColor", 0.0f, 0.0f, 0.0f);
        shader.SetFloat("emissionIntensity", 0.0f);
    }

    m_Mesh->Draw();

    if (mat) {
        mat->UnbindTextures();
    }
}

glm::mat4 GameObject::GetCameraViewMatrix() const {
    glm::vec3 pos = GetWorldPosition();
    glm::mat4 transform = GetTransformMatrix();
    // В OpenGL камера смотрит в направлении -Z (по стандарту)
    glm::vec3 forward = -glm::normalize(glm::vec3(transform[2])); // ось Z с минусом
    glm::vec3 up = glm::normalize(glm::vec3(transform[1]));
    return glm::lookAt(pos, pos + forward, up);
}

glm::mat4 GameObject::GetCameraProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(m_CameraFOV), aspectRatio, m_CameraNear, m_CameraFar);
}

void GameObject::AddRigidBody(float mass) {
    if (mass <= 0.0f) mass = 1.0f;
    m_mass = mass;
    UpdatePhysicsBody();   // создаст динамическое тело
    std::cout << "[Physics] " << m_Name << " became dynamic, mass=" << m_mass << std::endl;
}

void GameObject::RemoveRigidBody() {
    m_mass = 0.0f;          // статическое тело
    UpdatePhysicsBody();    // пересоздаст тело как статическое
    std::cout << "[Physics] " << m_Name << " became static" << std::endl;
}

void GameObject::SetColliderType(ColliderType type) {
    if (m_collisionShape) delete m_collisionShape;
    m_colliderType = type;
    glm::vec3 scale = GetScale();
    std::cout << "SetColliderType: " << type << " scale=(" << scale.x << "," << scale.y << "," << scale.z << ")" << std::endl;
    
    switch (type) {
        case COLLIDER_BOX:
            m_collisionShape = new btBoxShape(btVector3(scale.x * 0.5f, scale.y * 0.5f, scale.z * 0.5f));
            break;
        case COLLIDER_SPHERE:
            m_collisionShape = new btSphereShape(scale.x * 0.5f);
            break;
        case COLLIDER_CAPSULE:
            m_collisionShape = new btCapsuleShape(scale.x * 0.5f, scale.y - scale.x);
            break;
        default:
            m_collisionShape = nullptr;
            break;
    }
    
    // Обновляем физическое тело (создаём/пересоздаём)
    UpdatePhysicsBody();
}

void GameObject::SetFriction(float friction) {
    m_friction = friction;
    if (m_rigidBody) {
        m_rigidBody->setFriction(m_friction);
    }
}

void GameObject::SetRestitution(float restitution) {
    m_restitution = restitution;
    if (m_rigidBody) {
        m_rigidBody->setRestitution(m_restitution);
    }
}

void GameObject::SetRollingFriction(float rollingFriction) {
    m_rollingFriction = rollingFriction;
    if (m_rigidBody) {
        m_rigidBody->setRollingFriction(m_rollingFriction);
    }
}

void GameObject::SetLinearDamping(float damping) {
    m_linearDamping = damping;
    if (m_rigidBody) {
        m_rigidBody->setDamping(m_linearDamping, m_angularDamping);
    }
}

void GameObject::SetAngularDamping(float damping) {
    m_angularDamping = damping;
    if (m_rigidBody) {
        m_rigidBody->setDamping(m_linearDamping, m_angularDamping);
    }
}

void GameObject::SyncTransformToPhysics() {
    if (!m_rigidBody) return;
    btTransform trans;
    m_rigidBody->getMotionState()->getWorldTransform(trans);
    btVector3 pos = trans.getOrigin();
    SetPosition(glm::vec3(pos.x(), pos.y(), pos.z()));
    std::cout << "SyncTransformToPhysics: y = " << pos.y() << std::endl;
}

void GameObject::SyncPhysicsToTransform() {
    if (!m_rigidBody) return;
    btTransform trans;
    trans.setIdentity();
    glm::vec3 pos = GetWorldPosition();
    trans.setOrigin(btVector3(pos.x, pos.y, pos.z));
    glm::vec3 rot = GetRotation();
    trans.setRotation(btQuaternion(glm::radians(rot.y), glm::radians(rot.x), glm::radians(rot.z)));
    m_rigidBody->getMotionState()->setWorldTransform(trans);
    m_rigidBody->setCenterOfMassTransform(trans);
}

void GameObject::UpdatePhysicsBody() {
    if (!m_collisionShape) {
        // Нет коллайдера — удаляем тело из мира, если оно было
        if (m_rigidBody) {
            PhysicsWorld::GetInstance().RemoveRigidBody(m_rigidBody);
            delete m_rigidBody->getMotionState();
            delete m_rigidBody;
            m_rigidBody = nullptr;
        }
        return;
    }

    // Удаляем существующее тело, если есть
    if (m_rigidBody) {
        PhysicsWorld::GetInstance().RemoveRigidBody(m_rigidBody);
        delete m_rigidBody->getMotionState();
        delete m_rigidBody;
        m_rigidBody = nullptr;
    }

    // Создаём новое тело (статическое или динамическое)
    btTransform startTransform;
    startTransform.setIdentity();
    glm::vec3 pos = GetWorldPosition();
    startTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
    glm::vec3 rot = GetRotation();
    startTransform.setRotation(btQuaternion(glm::radians(rot.y), glm::radians(rot.x), glm::radians(rot.z)));

    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
    btVector3 inertia(0,0,0);
    if (m_mass != 0.0f) {
        m_collisionShape->calculateLocalInertia(m_mass, inertia);
    }
    btRigidBody::btRigidBodyConstructionInfo rbInfo(m_mass, motionState, m_collisionShape, inertia);
    m_rigidBody = new btRigidBody(rbInfo);
    PhysicsWorld::GetInstance().AddRigidBody(m_rigidBody);
    
    std::cout << "[Physics] UpdatePhysicsBody for " << m_Name 
              << ", mass=" << m_mass 
              << ", collider=" << (m_collisionShape ? "yes" : "no") 
              << std::endl;

    m_rigidBody->setFriction(m_friction);
    m_rigidBody->setRestitution(m_restitution);
    m_rigidBody->setRollingFriction(m_rollingFriction);
    m_rigidBody->setDamping(m_linearDamping, m_angularDamping);          
}

void GameObject::SaveInitialTransform() {
    m_initialPosition = GetPosition();
    m_initialRotation = GetRotation();
    m_initialScale = GetScale();
}

void GameObject::ResetToInitialTransform() {
    SetPosition(m_initialPosition);
    SetRotation(m_initialRotation);
    SetScale(m_initialScale);
    if (m_rigidBody) {
        SyncPhysicsToTransform();
        m_rigidBody->setLinearVelocity(btVector3(0,0,0));
        m_rigidBody->setAngularVelocity(btVector3(0,0,0));
    }
}
