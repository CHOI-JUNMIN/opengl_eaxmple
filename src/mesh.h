#pragma once
#include "common.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "texture.h"
#include "program.h"

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

CLASS_PTR(Material);
class Material
{
public:
    static MaterialUPtr Create()
    {
        return MaterialUPtr(new Material());
    }
    TexturePtr diffuse;
    TexturePtr specular;
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;
    glm::vec3 ambientColor;
    float shininess{64.0f};

    void SetToProgram(const Program *program) const;

private:
    Material() {}
};

CLASS_PTR(Mesh);
class Mesh
{
public:
    static MeshUPtr Create(
        const std::vector<Vertex> &vertices,
        const std::vector<uint32_t> &indices,
        uint32_t primitiveType);
    static MeshUPtr CreateBox();

    const VertexLayout *GetVertexLayout() const { return m_vertexLayout.get(); }
    BufferPtr GetVertexBuffer() const { return m_vertexBuffer; }
    BufferPtr GetIndexBuffer() const { return m_indexBuffer; }

    void SetMaterial(MaterialPtr material) { m_material = material; }
    MaterialPtr GetMaterial() const { return m_material; }

    void Draw(const Program* program) const;

    void SetNodeName(const std::string &name);
    const std::string &GetNodeName() const;

private:
    Mesh() {}
    void Init(
        const std::vector<Vertex> &vertices,
        const std::vector<uint32_t> &indices,
        uint32_t primitiveType);

    uint32_t m_primitiveType{GL_TRIANGLES};
    VertexLayoutUPtr m_vertexLayout;
    BufferPtr m_vertexBuffer;
    BufferPtr m_indexBuffer;
    MaterialPtr m_material;
    std::string m_nodeName;
};