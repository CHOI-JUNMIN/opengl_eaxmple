#include "context.h"
#include <imgui.h>

void Context::Render()
{
    if (ImGui::Begin("ui window"))
    {
        if (ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor)))
        {
            glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
        }
        ImGui::Separator();
        ImGui::DragFloat3("camera pos", glm::value_ptr(m_cameraPos), 0.01f);
        ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.5f);
        ImGui::DragFloat("camera pitch", &m_cameraPitch, 0.5f, -89.0f, 89.0f);
        ImGui::Separator();
        if (ImGui::Button("reset camera"))
        {
            m_cameraYaw = 0.0f;
            m_cameraPitch = 0.0f;
            m_cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        }
        ImGui::PushItemWidth(120);
        ImGui::BeginGroup();
        ImGui::DragFloat("J1", &j1, 0.1f, -180.0f, 180.0f);
        ImGui::SameLine();
        ImGui::DragFloat("J2", &j2, 0.1f, -180.0f, 180.0f);
        ImGui::DragFloat("J3", &j3, 0.1f, -180.0f, 180.0f);
        ImGui::SameLine();
        ImGui::DragFloat("J4", &j4, 0.1f, -180.0f, 180.0f);
        ImGui::DragFloat("J5", &j5, 0.1f, -180.0f, 180.0f);
        ImGui::SameLine();
        ImGui::DragFloat("J6", &j6, 0.1f, -180.0f, 180.0f);
        ImGui::EndGroup(); // 그룹 끝
        ImGui::PopItemWidth();
        if (ImGui::Button("reset robot"))
        {
            j1 = 0.0f;
            j2 = 0.0f;
            j3 = 0.0f;
            j4 = 0.0f;
            j5 = 0.0f;
            j6 = 0.0f;
        }
        ImGui::Separator();
        ImGui::PushItemWidth(80);
        ImGui::InputFloat("x", &targetPosition.x);
        ImGui::SameLine();
        ImGui::InputFloat("y", &targetPosition.y);
        ImGui::SameLine();
        ImGui::InputFloat("z", &targetPosition.z);
        // ImGui::Checkbox("animation", &m_animation);
    }
    ImGui::End();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    m_cameraFront =
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f); // 맨뒤에 1이면 점, 0이면 벡터 0을 집어넣으면 평행이동이 안됨

    auto projection = glm::perspective(glm::radians(45.0f), (float)m_width / (float)m_height, 0.01f, 30.0f);
    auto view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);

    glm::mat4 gridTransform = glm::mat4(1.0f);
    RenderGrid(view, projection, gridTransform);

    glm::mat4 modelTransform = glm::mat4(1.0f);
    glm::mat4 modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.001f, 0.001f, 0.001f));
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));

    m_program->Use();
    
    m_model->SetAngle1(j1);
    m_model->SetAngle2(j2);
    m_model->SetAngle3(j3);
    m_model->SetAngle4(j4);
    m_model->SetAngle5(j5);
    m_model->SetAngle6(j6);

    
    m_model->NewAngle1(targetPosition.x);
    m_model->NewAngle2(targetPosition.y);
    m_model->NewAngle3(targetPosition.z);
    

    m_program->SetUniform("transform", projection * view * modelTransform);
    m_program->SetUniform("modelTransform", modelTransform);
    m_program->SetUniform("modelscale", modelMatrix);
    m_program->SetUniform("rotation", rotation);

    m_model->Draw(m_program.get());
}