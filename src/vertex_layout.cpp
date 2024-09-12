#include "vertex_layout.h"

VertexLayoutUPtr VertexLayout::Create()
{
    auto vertexLayout = VertexLayoutUPtr(new VertexLayout());
    vertexLayout->Init();
    return std::move(vertexLayout);
}

VertexLayout::~VertexLayout()
{
    if (m_vertexArrayObject)
    {
        glDeleteVertexArrays(1, &m_vertexArrayObject);
        m_vertexArrayObject = 0; 
    }
}

void VertexLayout::Bind() const
{
    static GLuint currentlyBoundVAO = 0;
    if (currentlyBoundVAO != m_vertexArrayObject)
    {
        glBindVertexArray(m_vertexArrayObject);
        currentlyBoundVAO = m_vertexArrayObject;
    }
}

void VertexLayout::SetAttrib(
    uint32_t attribIndex, int count,
    uint32_t type, bool normalized,
    size_t stride, uint64_t offset) const
{
    glEnableVertexAttribArray(attribIndex);
    glVertexAttribPointer(attribIndex, count, type, normalized,
                          static_cast<GLsizei>(stride), reinterpret_cast<const void *>(offset));
}

void VertexLayout::Init()
{
    glGenVertexArrays(1, &m_vertexArrayObject);
    Bind();
}