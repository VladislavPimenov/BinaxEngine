#include "Graphics/Model.h"
#include <iostream>
#include <filesystem>

Model::Model(const std::string& path) {
    loadModel(path);
}

void Model::loadModel(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
    aiProcess_Triangulate |
    aiProcess_GenNormals |
    aiProcess_CalcTangentSpace |
    aiProcess_JoinIdenticalVertices |
    aiProcess_OptimizeMeshes
);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
        return;
    }

    // Определяем директорию модели для поиска текстур
    std::filesystem::path fsPath(path);
    m_Directory = fsPath.parent_path().string();

    processNode(scene->mRootNode, scene);
    std::cout << "Model loaded: " << path << ", meshes: " << m_Meshes.size() << std::endl;
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_Meshes.push_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        processNode(node->mChildren[i], scene);
    }
}

std::shared_ptr<Mesh> Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    auto material = std::make_shared<Material>();

    // Вершины
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vertex;
        vertex.Position[0] = mesh->mVertices[i].x;
        vertex.Position[1] = mesh->mVertices[i].y;
        vertex.Position[2] = mesh->mVertices[i].z;
        if (mesh->HasNormals()) {
            vertex.Normal[0] = mesh->mNormals[i].x;
            vertex.Normal[1] = mesh->mNormals[i].y;
            vertex.Normal[2] = mesh->mNormals[i].z;
        }
        if (mesh->mTextureCoords[0]) {
            vertex.TexCoords[0] = mesh->mTextureCoords[0][i].x;
            vertex.TexCoords[1] = mesh->mTextureCoords[0][i].y;
        } else {
            vertex.TexCoords[0] = vertex.TexCoords[1] = 0.0f;
        }
        if (mesh->HasTangentsAndBitangents()) {
            vertex.Tangent[0] = mesh->mTangents[i].x;
            vertex.Tangent[1] = mesh->mTangents[i].y;
            vertex.Tangent[2] = mesh->mTangents[i].z;
        } else {
            vertex.Tangent[0] = vertex.Tangent[1] = vertex.Tangent[2] = 0.0f;
        }
        vertices.push_back(vertex);
    }

    // Индексы
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j)
            indices.push_back(face.mIndices[j]);
    }

    // Материал
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];
        processMaterial(aiMat, m_Directory, material);
    }

    auto newMesh = std::make_shared<Mesh>(vertices, indices);
    newMesh->SetMaterial(material);

    // Устанавливаем имя меша (исправлено)
    std::string meshName = mesh->mName.C_Str();
    if (meshName.empty()) {
        meshName = "Mesh_" + std::to_string(m_Meshes.size());
    }
    newMesh->SetName(meshName);

    return newMesh;
}

void Model::processMaterial(aiMaterial* aiMat, const std::string& directory, std::shared_ptr<Material> material) {
    aiString path;

    if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
        material->LoadDiffuseTexture(directory + "/" + path.C_Str());
    }
    if (aiMat->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS) {
        material->LoadNormalTexture(directory + "/" + path.C_Str());
    }
    if (aiMat->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &path) == AI_SUCCESS) {
        material->LoadRoughnessTexture(directory + "/" + path.C_Str());
    }
    if (aiMat->GetTexture(aiTextureType_METALNESS, 0, &path) == AI_SUCCESS) {
        material->LoadMetallicTexture(directory + "/" + path.C_Str());
    }
    if (aiMat->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &path) == AI_SUCCESS) {
        material->LoadAOTexture(directory + "/" + path.C_Str());
    }

    aiColor3D color;
    if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
        material->albedo = glm::vec3(color.r, color.g, color.b);
    }
    float shininess, metallic, roughness;
    if (aiMat->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
        material->roughness = 1.0f - (shininess / 100.0f);
    }
    if (aiMat->Get(AI_MATKEY_METALLIC_FACTOR, metallic) == AI_SUCCESS) {
        material->metallic = metallic;
    }
    if (aiMat->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) == AI_SUCCESS) {
        material->roughness = roughness;
    }
}