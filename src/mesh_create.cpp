#include "mesh.h"

MeshUPtr Mesh::Create(
    const std::vector<Vertex> &vertices,
    const std::vector<uint32_t> &indices,
    uint32_t primitiveType)
{
    auto mesh = MeshUPtr(new Mesh());
    mesh->Init(vertices, indices, primitiveType);
    return std::move(mesh);
}

void Mesh::Init(
    const std::vector<Vertex> &vertices,
    const std::vector<uint32_t> &indices,
    uint32_t primitiveType)
{
    m_vertexLayout = VertexLayout::Create();
    if (!m_vertexLayout)
    {
        return;
    }

    m_vertexBuffer = Buffer::CreateWithData(
        GL_ARRAY_BUFFER, GL_STATIC_DRAW,
        vertices.data(), sizeof(Vertex), vertices.size());
    if (!m_vertexBuffer)
    {
        return;
    }

    m_indexBuffer = Buffer::CreateWithData(
        GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW,
        indices.data(), sizeof(uint32_t), indices.size());
    if (!m_indexBuffer)
    {
        return;
    }

    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, false, sizeof(Vertex), 0);
    m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, false, sizeof(Vertex), offsetof(Vertex, normal));
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, false, sizeof(Vertex), offsetof(Vertex, texCoord));

    // primitive type 저장
    m_primitiveType = primitiveType;
}