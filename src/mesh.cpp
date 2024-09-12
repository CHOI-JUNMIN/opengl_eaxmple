#include "mesh.h"

// 메시 생성 부분은 별도 파일로 분리되었으나, include는 유지됩니다.

void Mesh::SetNodeName(const std::string &name)
{
    m_nodeName = name;
}

const std::string &Mesh::GetNodeName() const
{
    return m_nodeName;
}