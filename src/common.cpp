#include "common.h"
#include <fstream>
#include <sstream>

std::optional<std::string> LoadTextFile(const std::string &filename)
{
    std::ifstream fin(filename);
    if (!fin.is_open())
    {
        SPDLOG_ERROR("failed to open file: {}", filename);
        return {};
    }

    std::string line;
    std::stringstream text;

    // 파일을 한 줄씩 읽음 (큰 파일에 유리)
    while (std::getline(fin, line))
    {
        text << line << '\n';
    }

    return text.str();
}

glm::vec3 GetAttenuationCoeff(float distance)
{
    static std::unordered_map<float, glm::vec3> attenuationCache;

    // 이전에 계산한 결과가 있다면 캐시에서 반환
    if (attenuationCache.find(distance) != attenuationCache.end())
    {
        return attenuationCache[distance];
    }

    const auto linear_coeff = glm::vec4(8.4523112e-05, 4.4712582e+00, -1.8516388e+00, 3.3955811e+01);
    const auto quad_coeff = glm::vec4(-7.6103583e-04, 9.0120201e+00, -1.1618500e+01, 1.0000464e+02);

    float kc = 1.0f;
    float d = 1.0f / distance;
    auto dvec = glm::vec4(1.0f, d, d * d, d * d * d);
    float kl = glm::dot(linear_coeff, dvec);
    float kq = glm::dot(quad_coeff, dvec);

    // 계산된 값을 캐시에 저장
    glm::vec3 result = glm::vec3(kc, glm::max(kl, 0.0f), glm::max(kq * kq, 0.0f));
    attenuationCache[distance] = result;

    return result;
}