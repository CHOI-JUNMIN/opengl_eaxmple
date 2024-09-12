#include "mesh.h"

void Mesh::Draw(const Program *program) const
{
    m_vertexLayout->Bind();

    if (m_material)
    {
        m_material->SetToProgram(program);
    }
    glDrawElements(m_primitiveType, m_indexBuffer->GetCount(), GL_UNSIGNED_INT, 0);
}

void Material::SetToProgram(const Program *program) const
{
    int textureCount = 0;

    if (diffuse)
    {
        glActiveTexture(GL_TEXTURE0 + textureCount);
        program->SetUniform("material.diffuse", textureCount);
        diffuse->Bind();
        textureCount++;
    }

    if (specular)
    {
        glActiveTexture(GL_TEXTURE0 + textureCount);
        program->SetUniform("material.specular", textureCount);
        specular->Bind();
        textureCount++;
    }

    // 기본 텍스처로 리셋
    glActiveTexture(GL_TEXTURE0);
    // material의 shininess 설정
    program->SetUniform("material.shininess", shininess);
}