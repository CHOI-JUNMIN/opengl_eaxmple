#include "shader.h"

ShaderUPtr Shader::CreateFromFile(const std::string &filename, GLenum shaderType)
{
    auto shader = ShaderUPtr(new Shader());
    if (!shader->LoadFile(filename, shaderType))
        return nullptr;
    return std::move(shader);
}

Shader::~Shader()
{
    if (m_shader)
    {
        glDeleteShader(m_shader);
    }
}

bool Shader::LoadFile(const std::string &filename, GLenum shaderType)
{
    auto result = LoadTextFile(filename);
    if (!result.has_value())
        return false;

    auto &code = result.value(); // auto가 value로 들어오는 타입을 보고 저절로 바꿔지지만 &는 안붙여지므로 작성해야함
    const char *codePtr = code.c_str();
    int32_t codeLength = (int32_t)code.length();

    m_shader = glCreateShader(shaderType);                                     // 멤버 변수에 저장
    glShaderSource(m_shader, 1, (const GLchar *const *)&codePtr, &codeLength); // 그 변수에 집어넣음
    glCompileShader(m_shader);                                                 // 알아서 컴파일

    // check compile error
    int success = 0; // 여긴 컴파일한거 오류 있나 없나 보는거
    glGetShaderiv(m_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[1024];
        glGetShaderInfoLog(m_shader, 1024, nullptr, infoLog);
        SPDLOG_ERROR("failed to compile shader: \"{}\"", filename);
        SPDLOG_ERROR("reason: {}", infoLog);
        return false;
    }
    return true;
}