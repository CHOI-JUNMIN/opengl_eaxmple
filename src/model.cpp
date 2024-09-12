#include "model.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

/*  텍스처있는3d모델전용
void Model::Draw(const Program* program) const  //모델 그리기
{
    for (auto &mesh : m_meshes)
    {
        mesh->Draw(program);
    }
}
*/

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