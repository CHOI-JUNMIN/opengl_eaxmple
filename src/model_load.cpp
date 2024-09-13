#include "model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

ModelUPtr Model::Load(const std::string &filename)
{
    auto model = ModelUPtr(new Model());
    if (!model->LoadByAssimp(filename))
        return nullptr;
    return std::move(model);
}

bool Model::LoadByAssimp(const std::string &filename)
{
    Assimp::Importer importer;
    auto scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        SPDLOG_ERROR("failed to load model: {}", filename);
        return false;
    }

    auto dirname = filename.substr(0, filename.find_last_of("/"));

    for (uint32_t i = 0; i < scene->mNumMaterials; i++)
    {
        auto material = scene->mMaterials[i];
        auto glMaterial = Material::Create();
        aiColor3D diffuseColor(0.8f, 0.8f, 0.8f);
        if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == AI_SUCCESS)
            glMaterial->diffuseColor = glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b);

        aiColor3D ambientColor(1.0f, 1.0f, 1.0f);
        if (material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor) == AI_SUCCESS)
            glMaterial->ambientColor = glm::vec3(ambientColor.r, ambientColor.g, ambientColor.b);

        aiColor3D specularColor(1.0f, 1.0f, 1.0f);
        if (material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor) == AI_SUCCESS)
            glMaterial->specularColor = glm::vec3(specularColor.r, specularColor.g, specularColor.b);

        float shininess = 32.0f;
        if (material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
            glMaterial->shininess = shininess;

        m_materials.push_back(std::move(glMaterial));
    }

    ProcessNode(scene->mRootNode, scene);
    return true;
}

void Model::ProcessNode(aiNode *node, const aiScene *scene)
{
    for (uint32_t i = 0; i < node->mNumMeshes; i++)
    {
        
        auto meshIndex = node->mMeshes[i];
        auto mesh = scene->mMeshes[meshIndex];
        auto glMesh = ProcessMesh(mesh, scene);
        glMesh->SetNodeName(node->mName.C_Str());
        m_meshes.push_back(std::move(glMesh));
        
        /*
         aiMatrix4x4 transformation = node->mTransformation;
         aiVector3D scaling;
         aiQuaternion rotation;
         aiVector3D position;

         transformation.Decompose(scaling, rotation, position);

         position.x *= 0.001;
         position.y *= 0.001;
         position.z *= 0.001;

         if (node->mParent)
         {
             SPDLOG_INFO("Node name: {}, Parent name: {}, Node position: x={}, y={}, z={}", node->mName.C_Str(), node->mParent->mName.C_Str(), position.x, position.y, position.z);
         }
         else
         {
             SPDLOG_INFO("Node name: {}, Parent name: None (Root Node), Node position: x={}, y={}, z={}", node->mName.C_Str(), position.x, position.y, position.z);
         }
        */
        /*
         for (uint32_t i = 0; i < node->mNumMeshes; i++)
         {

             auto meshIndex = node->mMeshes[i];
             auto mesh = scene->mMeshes[meshIndex];

             auto glMesh = ProcessMesh(mesh, scene);
             glMesh->SetNodeName(node->mName.C_Str()); // 현재 노드의 이름을 메쉬에 설정합니다.

             // 처리한 메쉬를 m_meshes 벡터에 추가합니다.
             m_meshes.push_back(std::move(glMesh));

             // ProcessMesh(mesh, scene);
         }

         for (uint32_t i = 0; i < node->mNumChildren; i++) // 자식 노드가 있으면 똑같이 ㄱㄱ
         {
             aiNode *childNode = node->mChildren[i];
             nodeHierarchy[node->mName.C_Str()].push_back(childNode->mName.C_Str());
             ProcessNode(node->mChildren[i], scene);
         }
         */
    }

    for (uint32_t i = 0; i < node->mNumChildren; i++)
    {
        aiNode *childNode = node->mChildren[i];
        nodeHierarchy[node->mName.C_Str()].push_back(childNode->mName.C_Str());
        ProcessNode(node->mChildren[i], scene);
    }
}

MeshUPtr Model::ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    vertices.resize(mesh->mNumVertices);

    for (uint32_t i = 0; i < mesh->mNumVertices; i++)
    {
        auto &v = vertices[i];
        v.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        v.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
    }

    std::vector<uint32_t> indices;
    indices.resize(mesh->mNumFaces * 3);

    for (uint32_t i = 0; i < mesh->mNumFaces; i++)
    {
        indices[3 * i] = mesh->mFaces[i].mIndices[0];
        indices[3 * i + 1] = mesh->mFaces[i].mIndices[1];
        indices[3 * i + 2] = mesh->mFaces[i].mIndices[2];
    }

    auto glMesh = Mesh::Create(vertices, indices, GL_TRIANGLES);
    if (mesh->mMaterialIndex >= 0)
        glMesh->SetMaterial(m_materials[mesh->mMaterialIndex]);

    return glMesh;
}
