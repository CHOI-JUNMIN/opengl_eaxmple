#include "model.h"

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
                SPDLOG_INFO("ambient Color Loaded: {}, {}, {}", ambientColor.r, ambientColor.g, ambientColor.b);
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

    ProcessNode(scene->mRootNode, scene); // 루트 노드부터 시작해 씬을 재귀적으로 처리하는 ProcessNode 함수를 호출
    // ShutdownAssimpLogger();
    return true;
}

    void Model::ProcessNode(aiNode * node, const aiScene *scene) // 현재 노드에 포함된 메쉬를 반복하면서 처리
    {
        for (uint32_t i = 0; i < node->mNumMeshes; i++)
        {
            auto meshIndex = node->mMeshes[i];
            auto mesh = scene->mMeshes[meshIndex];
            ProcessMesh(mesh, scene);
        }

        for (uint32_t i = 0; i < node->mNumChildren; i++) // 자식 노드가 있으면 똑같이 ㄱㄱ
        {
            ProcessNode(node->mChildren[i], scene);
        }
}

void Model::ProcessMesh(aiMesh *mesh, const aiScene *scene)  //메쉬 데이터 처리
{
    SPDLOG_INFO("process mesh: {}, #vert: {}, #face: {}", mesh->mName.C_Str(), mesh->mNumVertices, mesh->mNumFaces);

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
        glMesh->SetMaterial(m_materials[mesh->mMaterialIndex]);

    m_meshes.push_back(std::move(glMesh));
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
glm::vec3 lightPos(0.0f, 0.1f, 2.0f);


void Model::Draw(const Program *program) const
{
    for (const auto &mesh : m_meshes)
    {
        // 메쉬에 연결된 재질을 가져옴
        auto material = mesh->GetMaterial();
        // 셰이더 프로그램에 재질 정보 전달
        program->Use();
        program->SetUniform("materialdiffuse", material->diffuseColor);
        program->SetUniform("materialambient", material->ambientColor);
        program->SetUniform("materialspecular", material->specularColor);
        program->SetUniform("materialshininess", material->shininess);

        program->SetUniform("lightDiffuse", glm::vec3(1.0f, 1.0f, 1.0f));  // 확산광 색상
        program->SetUniform("lightSpecular", glm::vec3(0.0f, 0.0f, 0.0f)); // 반사광 색상
        program->SetUniform("Lightambient", glm::vec3(0.3f, 0.3f, 0.3f));  // 주변광
        program->SetUniform("lightPosition", lightPos);

        // 메쉬 그리기
        mesh->Draw(program);
    }
}
