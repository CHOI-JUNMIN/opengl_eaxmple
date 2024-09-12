#include "image.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

ImageUPtr Image::Load(const std::string &filepath)
{
    //stbi_set_flip_vertically_on_load(true); y축방향 이미지
    auto image = ImageUPtr(new Image());
    if (!image->LoadWithStb(filepath))
        return nullptr;
    return std::move(image);
}

Image::~Image()
{
    if (m_data)
    {
        stbi_image_free(m_data); 
        m_data = nullptr;        
    }
}

bool Image::LoadWithStb(const std::string &filepath)
{
    static std::unordered_map<std::string, uint8_t *> imageCache;

    if (imageCache.find(filepath) != imageCache.end())
    {
        m_data = imageCache[filepath];
        return true;
    }

    m_data = stbi_load(filepath.c_str(), &m_width, &m_height, &m_channelCount, 0);
    if (!m_data)
    {
        SPDLOG_ERROR("failed to load image: {}", filepath);
        return false;
    }
    imageCache[filepath] = m_data;
    return true;
}

ImageUPtr Image::Create(int width, int height, int channelCount)
{
    auto image = ImageUPtr(new Image());
    if (!image->Allocate(width, height, channelCount))
        return nullptr;
    return std::move(image);
}

bool Image::Allocate(int width, int height, int channelCount)
{
    m_width = width;
    m_height = height;
    m_channelCount = channelCount;

    m_data = (uint8_t *)malloc(m_width * m_height * m_channelCount);
    if (!m_data)
    {
        SPDLOG_ERROR("failed to allocate image data.");
        return false;
    }
    return true;
}

void Image::SetCheckImage(int gridX, int gridY)
{
    for (int j = 0; j < m_height; j++)
    {
        for (int i = 0; i < m_width; i++)
        {
            int pos = (j * m_width + i) * m_channelCount;
            bool even = ((i / gridX) + (j / gridY)) % 2 == 0;
            uint8_t value = even ? 255 : 0;
            for (int k = 0; k < m_channelCount; k++)
                m_data[pos + k] = value;
            if (m_channelCount > 3)
                m_data[3] = 255;
        }
    }
}

ImageUPtr Image::CreateSingleColorImage(int width, int height, const glm::vec4 &color)
{
    glm::vec4 clamped = glm::clamp(color * 255.0f, 0.0f, 255.0f);
    uint8_t rgba[4] = {
        (uint8_t)clamped.r,
        (uint8_t)clamped.g,
        (uint8_t)clamped.b,
        (uint8_t)clamped.a,
    };

    auto image = Create(width, height, 4);
    if (!image)
    {
        SPDLOG_ERROR("failed to create image.");
        return nullptr;
    }

    for (int i = 0; i < width * height; i++)
    {
        std::copy(rgba, rgba + 4, image->m_data + 4 * i);
    }
    return std::move(image);
}