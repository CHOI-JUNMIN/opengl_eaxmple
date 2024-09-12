#include "model.h"
#include <glm/gtc/matrix_transform.hpp>

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

    glm::mat4 j1ModelMatrix = glm::rotate(modelMatrix, glm::radians(m_angle1), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::vec3 j2(0.0, 0.087, 0.135);
    glm::mat4 j2TranslateMatrix = glm::translate(j1ModelMatrix, j2);
    glm::mat4 j2RotateMatrix = glm::rotate(j2TranslateMatrix, glm::radians(m_angle2), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 j2ModelMatrix = glm::translate(j2RotateMatrix, -j2);

    glm::vec3 j3(0.0, 0.17, 0.546);
    glm::mat4 j3TranslateMatrix = glm::translate(j2ModelMatrix, j3);
    glm::mat4 j3RotateMatrix = glm::rotate(j3TranslateMatrix, glm::radians(m_angle3), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 j3ModelMatrix = glm::translate(j3RotateMatrix, -j3);

    glm::vec3 j4(0.0, -0.006, 0.63);
    glm::mat4 j4TranslateMatrix = glm::translate(j3ModelMatrix, j4);
    glm::mat4 j4RotateMatrix = glm::rotate(j4TranslateMatrix, glm::radians(m_angle4), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 j4ModelMatrix = glm::translate(j4RotateMatrix, -j4);

    glm::vec3 j5(0.0, 0.085, 0.914);
    glm::mat4 j5TranslateMatrix = glm::translate(j4ModelMatrix, j5);
    glm::mat4 j5RotateMatrix = glm::rotate(j5TranslateMatrix, glm::radians(m_angle5), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 j5ModelMatrix = glm::translate(j5RotateMatrix, -j5);

    glm::vec3 j6(0.0, -0.0062, 0.975);
    glm::mat4 j6TranslateMatrix = glm::translate(j5ModelMatrix, j6);
    glm::mat4 j6RotateMatrix = glm::rotate(j6TranslateMatrix, glm::radians(m_angle6), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 j6ModelMatrix = glm::translate(j6RotateMatrix, -j6);

    glm::vec3 lightPos(0.0f, 1.0f, 1.0f);

    for (const auto &mesh : m_meshes)
    {
        auto material = mesh->GetMaterial();

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

        program->SetUniform("lightDiffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        program->SetUniform("lightSpecular", glm::vec3(0.0f, 0.0f, 0.0f));
        program->SetUniform("Lightambient", glm::vec3(0.3f, 0.3f, 0.3f));
        program->SetUniform("lightPosition", lightPos);

        program->SetUniform("modelMatrix", modelMatrix);
        mesh->Draw(program);
    }
}