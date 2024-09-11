#include "model.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
/*
#include <assimp/Logger.hpp>
#include <assimp/DefaultLogger.hpp>

void InitAssimpLogger()
{
    Assimp::DefaultLogger::create("log.txt", Assimp::Logger::VERBOSE, aiDefaultLogStream_FILE);

    // 로그 메시지 설정
    Assimp::DefaultLogger::get()->info("Assimp Logger Initialized.");
}

// 로그 종료 함수 추가
void ShutdownAssimpLogger()
{
    Assimp::DefaultLogger::kill();
}
*/

ModelUPtr Model::Load(const std::string &filename)    //모델로드 함수
{
    auto model = ModelUPtr(new Model());
    if (!model->LoadByAssimp(filename))
        return nullptr;
    return std::move(model);
}

bool Model::LoadByAssimp(const std::string &filename)   //Assimp라이브러리 사용 모델로드
{
    //InitAssimpLogger();

    Assimp::Importer importer;
    auto scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals); // 모델 파일로드
    //                                          모든메쉬 삼각형 변환    UV좌표 상하 반전
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    { // 씬이 제대로 로드되지 않았거나, 불완전하거나, 루트 노드가 없는 경우
        SPDLOG_ERROR("failed to load model: {}", filename);
        return false;
    }

    auto dirname = filename.substr(0, filename.find_last_of("/")); // 파일 이름에서 디렉토리 이름을 추출

    // 텍스처가 있는지 확인하고, 있다면 경로를 가져와 이미지를 로드
    // 이를 기반으로 텍스처 객체를 생성
    auto LoadTexture = [&](aiMaterial *material, aiTextureType type) -> TexturePtr
    {
        if (material->GetTextureCount(type) <= 0)
            return nullptr;
        aiString filepath;
        material->GetTexture(type, 0, &filepath);
        auto image = Image::Load(fmt::format("{}/{}", dirname, filepath.C_Str()));
        if (!image)
            return nullptr;
        return Texture::CreateFromImage(image.get());
    };
    /*  //텍스처있는 3d모델전용
     for (uint32_t i = 0; i < scene->mNumMaterials; i++)  //scene에 포함된 모든 재질을 반복하면서 로드
     {
         auto material = scene->mMaterials[i];
         auto glMaterial = Material::Create();
         glMaterial->diffuse = LoadTexture(material, aiTextureType_DIFFUSE);
         glMaterial->specular = LoadTexture(material, aiTextureType_SPECULAR);
         m_materials.push_back(std::move(glMaterial));
     }
    */
        for (uint32_t i = 0; i < scene->mNumMaterials; i++)
        {
            auto material = scene->mMaterials[i];
            auto glMaterial = Material::Create();

            // Diffuse 색상 로드 (MTL 파일의 Kd 값)
            aiColor3D diffuseColor(0.8f, 0.8f, 0.8f); // 기본값 설정
            if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == AI_SUCCESS)
            {
                glMaterial->diffuseColor = glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b);
               // SPDLOG_INFO("Diffuse Color Loaded: {}, {}, {}", diffuseColor.r, diffuseColor.g, diffuseColor.b);
            }

            aiColor3D ambientColor(1.0f, 1.0f, 1.0f); // 기본값 설정
            if (material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor) == AI_SUCCESS)
            {
                glMaterial->ambientColor = glm::vec3(ambientColor.r, ambientColor.g, ambientColor.b);
               // SPDLOG_INFO("ambient Color Loaded: {}, {}, {}", ambientColor.r, ambientColor.g, ambientColor.b);
            }

            // Specular 색상 로드 (MTL 파일의 Ks 값)
            aiColor3D specularColor(1.0f, 1.0f, 1.0f); // 기본값 설정
            if (material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor) == AI_SUCCESS)
            {
                glMaterial->specularColor = glm::vec3(specularColor.r, specularColor.g, specularColor.b);
               // SPDLOG_INFO("Specular Color Loaded: {}, {}, {}", specularColor.r, specularColor.g, specularColor.b);
            }

            // Shininess 로드 (MTL 파일의 Ns 값)
            float shininess = 32.0f; // 기본값 설정
            if (material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
            {
                glMaterial->shininess = shininess;
               // SPDLOG_INFO("Shininess Loaded: {}", shininess);
            }

            m_materials.push_back(std::move(glMaterial));
        }

    ProcessNode(scene->mRootNode, scene); // 루트 노드부터 시작해 씬을 재귀적으로 처리하는 ProcessNode 함수를 호출);
    // ShutdownAssimpLogger();
    return true;
}

    void Model::ProcessNode(aiNode * node, const aiScene *scene) // 현재 노드에 포함된 메쉬를 반복하면서 처리
    {
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

        for (uint32_t i = 0; i < node->mNumMeshes; i++)
        {

            auto meshIndex = node->mMeshes[i];
            auto mesh = scene->mMeshes[meshIndex];

            auto glMesh = ProcessMesh(mesh, scene);
            glMesh->SetNodeName(node->mName.C_Str()); // 현재 노드의 이름을 메쉬에 설정합니다.

            // 처리한 메쉬를 m_meshes 벡터에 추가합니다.
            m_meshes.push_back(std::move(glMesh));

            //ProcessMesh(mesh, scene);
        }

        for (uint32_t i = 0; i < node->mNumChildren; i++) // 자식 노드가 있으면 똑같이 ㄱㄱ
        {
            aiNode *childNode = node->mChildren[i];
            nodeHierarchy[node->mName.C_Str()].push_back(childNode->mName.C_Str());
            ProcessNode(node->mChildren[i], scene);
        }
}

MeshUPtr Model::ProcessMesh(aiMesh *mesh, const aiScene *scene) // 메쉬 데이터 처리
{
    //SPDLOG_INFO("process mesh: {}, #vert: {}, #face: {}", mesh->mName.C_Str(), mesh->mNumVertices, mesh->mNumFaces);

    std::vector<Vertex> vertices;    //정점(vertex) 데이터 처리
    vertices.resize(mesh->mNumVertices);

    for (uint32_t i = 0; i < mesh->mNumVertices; i++)
    {
        auto &v = vertices[i];
        v.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        v.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        //v.texCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
    }

    std::vector<uint32_t> indices; // 메쉬의 인덱스 데이터를 처리합니다. 각 면(Face)에서 세 개의 인덱스를 가져와 삼각형을 구성
    indices.resize(mesh->mNumFaces * 3);

    for (uint32_t i = 0; i < mesh->mNumFaces; i++)
    {
        indices[3 * i] = mesh->mFaces[i].mIndices[0];
        indices[3 * i + 1] = mesh->mFaces[i].mIndices[1];
        indices[3 * i + 2] = mesh->mFaces[i].mIndices[2];
    }

    auto glMesh = Mesh::Create(vertices, indices, GL_TRIANGLES); // 정점(Vertex)와 인덱스(Index) 데이터를 사용해 OpenGL 메쉬 객체를 생성

    if (mesh->mMaterialIndex >= 0) // 메쉬에 재질이 연결되어 있으면, 해당 재질을 메쉬에 설정
    {
        glMesh->SetMaterial(m_materials[mesh->mMaterialIndex]);
    }
    return glMesh;
    //m_meshes.push_back(std::move(glMesh));
}
/*  텍스처있는3d모델전용
void Model::Draw(const Program* program) const  //모델 그리기
{
    for (auto &mesh : m_meshes)
    {
        mesh->Draw(program);
    }
}
*/
void Model::SetAngle1(float newAngle)
{
    m_angle1 = newAngle;  
}

void Model::SetAngle2(float newAngle)
{
    m_angle2 = newAngle;  
}

void Model::SetAngle3(float newAngle)
{
    m_angle3 = newAngle;  
}

void Model::SetAngle4(float newAngle)
{
    m_angle4 = newAngle;  
}

void Model::SetAngle5(float newAngle)
{
    m_angle5 = newAngle;  
}

void Model::SetAngle6(float newAngle)
{
    m_angle6 = newAngle;  
}

void Model::Draw(const Program *program) const
{
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    // j1
    glm::mat4 j1ModelMatrix = glm::rotate(modelMatrix, glm::radians(m_angle1), glm::vec3(0.0f, 0.0f, 1.0f));

    // j2
    glm::vec3 j2(0.0, 0.087, 0.135);
    glm::mat4 j2TranslateMatrix = glm::translate(j1ModelMatrix, j2);
    glm::mat4 j2RotateMatrix = glm::rotate(j2TranslateMatrix, glm::radians(m_angle2), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 j2ModelMatrix = glm::translate(j2RotateMatrix, -j2);

    // j3
    glm::vec3 j3(0.0, 0.17, 0.546);
    glm::mat4 j3TranslateMatrix = glm::translate(j2ModelMatrix, j3);
    glm::mat4 j3RotateMatrix = glm::rotate(j3TranslateMatrix, glm::radians(m_angle3), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 j3ModelMatrix = glm::translate(j3RotateMatrix, -j3);

    // j4
    glm::vec3 j4(0.0, -0.006, 0.63);
    glm::mat4 j4TranslateMatrix = glm::translate(j3ModelMatrix, j4);
    glm::mat4 j4RotateMatrix = glm::rotate(j4TranslateMatrix, glm::radians(m_angle4), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 j4ModelMatrix = glm::translate(j4RotateMatrix, -j4);

    // j5
    glm::vec3 j5(0.0, 0.085, 0.914);
    glm::mat4 j5TranslateMatrix = glm::translate(j4ModelMatrix, j5);
    glm::mat4 j5RotateMatrix = glm::rotate(j5TranslateMatrix, glm::radians(m_angle5), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 j5ModelMatrix = glm::translate(j5RotateMatrix, -j5);

    // j6
    glm::vec3 j6(0.0, -0.0062, 0.975);
    glm::mat4 j6TranslateMatrix = glm::translate(j5ModelMatrix, j6);
    glm::mat4 j6RotateMatrix = glm::rotate(j6TranslateMatrix, glm::radians(m_angle6), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 j6ModelMatrix = glm::translate(j6RotateMatrix, -j6);

    glm::vec3 lightPos(0.0f, 1.0f, 1.0f);

    for (const auto &mesh : m_meshes)
    {

        //  메쉬에 연결된 재질을 가져옴
        auto material = mesh->GetMaterial();
        // 셰이더 프로그램에 재질 정보 전달

        if (IsChildOf(mesh->GetNodeName(), "j1"))
        {
            modelMatrix = j1ModelMatrix;
        }

        if (IsChildOf(mesh->GetNodeName(), "j2"))
        {
            modelMatrix = j2ModelMatrix;
        }

        if (IsChildOf(mesh->GetNodeName(), "j3"))
        {
            modelMatrix = j3ModelMatrix;
        }

        if (IsChildOf(mesh->GetNodeName(), "j4"))
        {
            modelMatrix = j4ModelMatrix;
        }

        if (IsChildOf(mesh->GetNodeName(), "j5"))
        {
            modelMatrix = j5ModelMatrix;
        }

        if (IsChildOf(mesh->GetNodeName(), "j6"))
        {
            modelMatrix = j6ModelMatrix;
        }

        program->Use();
        program->SetUniform("materialdiffuse", material->diffuseColor);
        program->SetUniform("materialambient", material->ambientColor);
        program->SetUniform("materialspecular", material->specularColor);
        program->SetUniform("materialshininess", material->shininess);

        program->SetUniform("lightDiffuse", glm::vec3(1.0f, 1.0f, 1.0f));  // 확산광 색상
        program->SetUniform("lightSpecular", glm::vec3(0.0f, 0.0f, 0.0f)); // 반사광 색상
        program->SetUniform("Lightambient", glm::vec3(0.3f, 0.3f, 0.3f));  // 주변광
        program->SetUniform("lightPosition", lightPos);

        program->SetUniform("modelMatrix", modelMatrix);
        std::string nodename = mesh->GetNodeName();
        
        if (nodename.find("j"))
        {
            mesh->Draw(program);
        }
        
        //mesh->Draw(program);
    }
}

bool Model::IsChildOf(const std::string &nodeName, const std::string &parentNodeName) const
{
    if (this->nodeHierarchy.find(parentNodeName) != this->nodeHierarchy.end())
    {
        // operator[] 대신 at()을 사용하여 const 접근 허용
        const auto &children = this->nodeHierarchy.at(parentNodeName);
        return std::find(children.begin(), children.end(), nodeName) != children.end();
    }
    return false;
}