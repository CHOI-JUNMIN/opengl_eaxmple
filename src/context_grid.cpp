#include "context.h"

void Context::CreateGrid(int gridSize, float boxLength)
{
    m_gridVertices.clear(); // 기존 그리드 데이터를 초기화

    for (int i = -gridSize; i <= gridSize; i++)
        if (i % 5 == 0) // 5의 배수인 경우 굵은 선 그리드에 추가
        {
            // 굵은 선 X 방향
            m_boldGridVertices.push_back(i * boxLength);
            m_boldGridVertices.push_back(0.0f);
            m_boldGridVertices.push_back(-gridSize * boxLength);

            m_boldGridVertices.push_back(i * boxLength);
            m_boldGridVertices.push_back(0.0f);
            m_boldGridVertices.push_back(gridSize * boxLength);

            // 굵은 선 Z 방향
            m_boldGridVertices.push_back(-gridSize * boxLength);
            m_boldGridVertices.push_back(0.0f);
            m_boldGridVertices.push_back(i * boxLength);

            m_boldGridVertices.push_back(gridSize * boxLength);
            m_boldGridVertices.push_back(0.0f);
            m_boldGridVertices.push_back(i * boxLength);
        }
        else // 일반 선 그리드에 추가
        {
            // 일반 그리드 X 방향
            m_gridVertices.push_back(i * boxLength);
            m_gridVertices.push_back(0.0f);
            m_gridVertices.push_back(-gridSize * boxLength);

            m_gridVertices.push_back(i * boxLength);
            m_gridVertices.push_back(0.0f);
            m_gridVertices.push_back(gridSize * boxLength);

            // 일반 그리드 Z 방향
            m_gridVertices.push_back(-gridSize * boxLength);
            m_gridVertices.push_back(0.0f);
            m_gridVertices.push_back(i * boxLength);

            m_gridVertices.push_back(gridSize * boxLength);
            m_gridVertices.push_back(0.0f);
            m_gridVertices.push_back(i * boxLength);
        }
}

void Context::RenderGrid(const glm::mat4 &view, const glm::mat4 &projection, const glm::mat4 &gridTransform)
{
    m_program1->Use(); // 셰이더 프로그램 사용

    // 그리드의 변환 행렬을 별도로 적용
    glm::mat4 scaleMatrix = glm::scale(gridTransform, glm::vec3(0.1f, 0.1f, 0.1f));
    auto transform = projection * view * scaleMatrix;

    m_program1->SetUniform("transform", transform);
    m_program1->SetUniform("gridColor", glm::vec3(0.1f, 0.1f, 0.1f));

    // 그리드 그리기
    glLineWidth(1.0f);
    glBindVertexArray(m_gridVAO);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_gridVertices.size() / 3));
    glBindVertexArray(0);

    glLineWidth(1.5f); // 굵은 선 굵기
    glBindVertexArray(m_boldGridVAO);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_boldGridVertices.size() / 3));
    glBindVertexArray(0);
}