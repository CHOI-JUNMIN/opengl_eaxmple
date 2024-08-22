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

private:
    Model() {}
    bool LoadByAssimp(const std::string &filename);   //모델 로드 함수
    void ProcessMesh(aiMesh *mesh, const aiScene *scene);  //aiMesh 객체처리 , aiScene은 Assimp가 로드한 전체 씬 데이터를 나타냄
    void ProcessNode(aiNode *node, const aiScene *scene); //aiNode 처리 , 노드를 재귀적으로 처리하면서 모델의 메쉬와 재질을 처리 

    std::vector<MeshPtr> m_meshes;   //메쉬저장벡터
    std::vector<MaterialPtr> m_materials; //재질저장 벡터
    std::map<std::string, MaterialPtr> m_materialMap; // 재질 이름을 키로 하고 재질 포인터를 값으로 하는 맵 재질을 파일에서 로드할때 이름으로 관리
};

#endif // __MODEL_H__
