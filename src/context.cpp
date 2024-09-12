#include "context.h"
#include "image.h"
#include <imgui.h>
#include "model.h"

ContextUPtr Context::Create()
{
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

void Context::Reshape(int width, int height)
{
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}

bool Context::Init()
{
    m_model = Model::Load("./model/m0609.3DS");
    if (!m_model)
        return false;

    m_simpleProgram = Program::Create("./shader/simple.vs", "./shader/simple.fs");
    if (!m_simpleProgram)
        return false;

    m_program = Program::Create("./shader/lighting.vs", "./shader/lighting.fs");
    if (!m_program)
        return false;

    m_program1 = Program::Create("./shader/grid.vs", "./shader/grid.fs");
    if (!m_program)
        return false;

    static bool gridInitialized = false;
    if (!gridInitialized)
    {
        int gridSize = 50;      // 그리드 크기
        float boxLength = 1.0f; // 각 그리드의 크기
        CreateGrid(gridSize, boxLength);
        gridInitialized = true;
    }

    // 그리드용 VAO와 VBO 설정 (한 번만 수행)
    glGenVertexArrays(1, &m_gridVAO);
    glGenBuffers(1, &m_gridVBO);
    glBindVertexArray(m_gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_gridVBO);
    glBufferData(GL_ARRAY_BUFFER, m_gridVertices.size() * sizeof(float), m_gridVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 굵은 그리드용 VAO와 VBO 설정 (한 번만 수행)
    glGenVertexArrays(1, &m_boldGridVAO);
    glGenBuffers(1, &m_boldGridVBO);
    glBindVertexArray(m_boldGridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_boldGridVBO);
    glBufferData(GL_ARRAY_BUFFER, m_boldGridVertices.size() * sizeof(float), m_boldGridVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    return true;
}