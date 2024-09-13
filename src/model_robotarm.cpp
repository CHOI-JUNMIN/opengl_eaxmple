#include "model.h"
#include <glm/gtc/matrix_transform.hpp>


void Model::NewAngle1(float x)
{
    targetX = x;
}

void Model::NewAngle2(float y)
{
    targetY = y;
}

void Model::NewAngle3(float z)
{
    targetZ = z;
}

glm::vec3 Model::ForwardKinematics(float q1, float q2, float q3, float q4, float q5, float q6)
{
    // 각 관절의 회전과 변환 행렬을 계산하고, 팔 끝단의 위치를 반환합니다.
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    // 각 관절의 회전과 변환
    glm::mat4 j1ModelMatrix = glm::rotate(modelMatrix, glm::radians(q1), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 j2ModelMatrix = glm::rotate(j1ModelMatrix, glm::radians(q2), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 j3ModelMatrix = glm::rotate(j2ModelMatrix, glm::radians(q3), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 j4ModelMatrix = glm::rotate(j3ModelMatrix, glm::radians(q4), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 j5ModelMatrix = glm::rotate(j4ModelMatrix, glm::radians(q5), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 j6ModelMatrix = glm::rotate(j5ModelMatrix, glm::radians(q6), glm::vec3(0.0f, 0.0f, 1.0f));

    // 최종 팔 끝단 위치 계산 (예시로 z 위치를 반환)
    glm::vec3 endEffectorPosition = glm::vec3(j6ModelMatrix[3]);

    return endEffectorPosition;
}

void Model::ComputeJacobian(float J[6][6], float q1, float q2, float q3, float q4, float q5, float q6)
{
    // 수치적 미분을 사용해 각도를 약간 변화시켜 자코비안 행렬의 각 요소를 계산
    float delta = 0.01f; // 작은 값 (미분 근사용)

    // 현재 끝단 위치
    glm::vec3 currentPos = ForwardKinematics(q1, q2, q3, q4, q5, q6);

    // ∂x/∂q1 계산
    glm::vec3 pos_q1 = ForwardKinematics(q1 + delta, q2, q3, q4, q5, q6);
    J[0][0] = (pos_q1.x - currentPos.x) / delta;
    J[1][0] = (pos_q1.y - currentPos.y) / delta;
    J[2][0] = (pos_q1.z - currentPos.z) / delta;

    // ∂x/∂q2 계산
    glm::vec3 pos_q2 = ForwardKinematics(q1, q2 + delta, q3, q4, q5, q6);
    J[0][1] = (pos_q2.x - currentPos.x) / delta;
    J[1][1] = (pos_q2.y - currentPos.y) / delta;
    J[2][1] = (pos_q2.z - currentPos.z) / delta;

    // 나머지 각도에 대해서도 동일한 방식으로 계산
    glm::vec3 pos_q3 = ForwardKinematics(q1, q2, q3 + delta, q4, q5, q6);
    J[0][2] = (pos_q3.x - currentPos.x) / delta;
    J[1][2] = (pos_q3.y - currentPos.y) / delta;
    J[2][2] = (pos_q3.z - currentPos.z) / delta;

    glm::vec3 pos_q4 = ForwardKinematics(q1, q2, q3, q4 + delta, q5, q6);
    J[0][3] = (pos_q4.x - currentPos.x) / delta;
    J[1][3] = (pos_q4.y - currentPos.y) / delta;
    J[2][3] = (pos_q4.z - currentPos.z) / delta;

    glm::vec3 pos_q5 = ForwardKinematics(q1, q2, q3, q4, q5 + delta, q6);
    J[0][4] = (pos_q5.x - currentPos.x) / delta;
    J[1][4] = (pos_q5.y - currentPos.y) / delta;
    J[2][4] = (pos_q5.z - currentPos.z) / delta;

    glm::vec3 pos_q6 = ForwardKinematics(q1, q2, q3, q4, q5, q6 + delta);
    J[0][5] = (pos_q6.x - currentPos.x) / delta;
    J[1][5] = (pos_q6.y - currentPos.y) / delta;
    J[2][5] = (pos_q6.z - currentPos.z) / delta;
}

void Model::CalculateIK(float targetX, float targetY, float targetZ)
{
    // 초기 각도 설정 (예: 0도로 시작)
    float q1 = 0.0f, q2 = 0.0f, q3 = 0.0f, q4 = 0.0f, q5 = 0.0f, q6 = 0.0f;

    // 목표 좌표와 오차 계산을 위한 반복문
    for (int iter = 0; iter < 1000; ++iter)
    {
        // 현재 팔 끝단의 위치 계산
        glm::vec3 currentPos = ForwardKinematics(q1, q2, q3, q4, q5, q6);

        // 목표 좌표와 현재 좌표 간의 오차 계산
        glm::vec3 error = glm::vec3(targetX, targetY, targetZ) - currentPos;
        if (glm::length(error) > 100.0f)
        {
            // 오차가 너무 클 경우 적정한 크기로 제한
            error = glm::normalize(error) * 100.0f;
        }

        // 오차가 매우 작으면(목표 좌표에 도달했으면) 반복 종료
        if (glm::length(error) < 0.001f)
            break;

        // 자코비안 행렬 계산
        float J[6][6];
        ComputeJacobian(J, q1, q2, q3, q4, q5, q6);

        // 자코비안 행렬의 역행렬 계산
        float J_inv[6][6] ;

        // 각도 변화량 계산 (자코비안 역행렬과 오차 벡터의 곱)
        float dq[6];
        for (int i = 0; i < 6; ++i)
        {
            dq[i] = 0;
            for (int j = 0; j < 3; ++j)
            {
                dq[i] += J_inv[i][j] * error[j]; // 오류 벡터와 역자코비안의 곱
            }
        }
        // 각 관절의 각도 업데이트
        q1 += dq[0];
        q2 += dq[1];
        q3 += dq[2];
        q4 += dq[3];
        q5 += dq[4];
        q6 += dq[5];
        SPDLOG_INFO("dq= {}", dq[0]);
    }

    // 최종 각도를 멤버 변수에 저장
    m_angle1 = glm::degrees(q1);
    m_angle2 = glm::degrees(q2);
    m_angle3 = glm::degrees(q3);
    m_angle4 = glm::degrees(q4);
    m_angle5 = glm::degrees(q5);
    m_angle6 = glm::degrees(q6);
    //SPDLOG_INFO("q1= {} q2= {} q3= {} q4= {} q5= {} q6= {}", q1, q2 , q3 ,q4 ,q5 ,q6);
    
}


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
    
    Model *nonConstThis = const_cast<Model *>(this);
    nonConstThis->CalculateIK(targetX, targetY, targetZ); // 예시 좌표로 호출

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
        std::string nodename = mesh->GetNodeName();
        if (nodename.find("j"))
        {
            mesh->Draw(program);
        }
    }
}