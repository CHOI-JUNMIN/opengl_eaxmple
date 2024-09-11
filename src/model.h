#ifndef __MODEL_H__
#define __MODEL_H__

#include "common.h"
#include "mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

CLASS_PTR(Model); // std::shared_ptr<Model>, std::unique_ptr<Model> 같은 포인터 유형을 만들기 위한 코드
class Model
{
public:
    static ModelUPtr Load(const std::string &filename);   //파일 이름 받아 모델로드 

    int GetMeshCount() const { return (int)m_meshes.size(); } // 모델에 포함된 메쉬의 개수를 반환 m_meshes에 저장
    MeshPtr GetMesh(int index) const { return m_meshes[index]; } // 주어진 인덱스에 해당하는 메쉬를 반환하는 함
    void Draw(const Program* program) const;   //모델 그림
    void UpdateNodeRotation(aiNode *node, const glm::vec3 &axis, float angle);
    void RotateNode(aiNode *node, const glm::vec3 &axis, float angle);

    bool IsChildOf(const std::string &nodeName, const std::string &parentNodeName) const;
    void DrawNode(const aiNode *node, glm::mat4 parentTransform, const Program *program) const;

    void SetAngle1(float newAngle);
    void SetAngle2(float newAngle);
    void SetAngle3(float newAngle);
    void SetAngle4(float newAngle);
    void SetAngle5(float newAngle);
    void SetAngle6(float newAngle);

    void SetTargetPosition(const glm::vec3 &targetPosition); // 목표 좌표를 설정하는 함수
    void rotateToTarget(const glm::vec3 &targetPosition);
    void rotateToTarget(const glm::vec3 &targetPosition) const;
    void updateModelMatrices(const glm::vec3 &deltaTheta);
    void someFunctionThatCallsRotateToTarget();

private:
    Model()
    {

    }
    bool LoadByAssimp(const std::string &filename);   //모델 로드 함수
    MeshUPtr ProcessMesh(aiMesh *mesh, const aiScene *scene); // aiMesh 객체처리 , aiScene은 Assimp가 로드한 전체 씬 데이터를 나타냄
    //void ProcessMesh(aiMesh *mesh, const aiScene *scene);
    void ProcessNode(aiNode *node, const aiScene *scene); //aiNode 처리 , 노드를 재귀적으로 처리하면서 모델의 메쉬와 재질을 처리 

    std::vector<MeshPtr> m_meshes;   //메쉬저장벡터
    std::vector<MaterialPtr> m_materials; //재질저장 벡터

    const aiScene *m_scene;
    //std::map<std::string, MaterialPtr> m_materialMap; // 재질 이름을 키로 하고 재질 포인터를 값으로 하는 맵 재질을 파일에서 로드할때 이름으로 관리
    std::unordered_map<std::string, std::vector<std::string>> nodeHierarchy;

    float m_angle1 = 0.0f;
    float m_angle2 = 0.0f;
    float m_angle3 = 0.0f;
    float m_angle4 = 0.0f;
    float m_angle5 = 0.0f;
    float m_angle6 = 0.0f;

    glm::mat4 j1ModelMatrix;
    glm::mat4 j2ModelMatrix;
    glm::mat4 j3ModelMatrix;
    glm::mat4 j4ModelMatrix;
    glm::mat4 j5ModelMatrix;
    glm::mat4 j6ModelMatrix;
    glm::vec3 m_targetPosition;
};
#endif // __MODEL_H__