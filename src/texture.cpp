#include "texture.h"

TextureUPtr Texture::CreateFromImage(const Image *image)
{
    auto texture = TextureUPtr(new Texture());
    texture->CreateTexture();
    texture->SetTextureFromImage(image);
    return std::move(texture);
}

Texture::~Texture()
{
    if (m_texture)
    {
        glDeleteTextures(1, &m_texture);
        m_texture = 0; 
    }
}

void Texture::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, m_texture);
}

void Texture::SetFilter(uint32_t minFilter, uint32_t magFilter) const
{
    glBindTexture(GL_TEXTURE_2D, m_texture); // 텍스처 바인딩
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

void Texture::SetWrap(uint32_t sWrap, uint32_t tWrap) const
{
    glBindTexture(GL_TEXTURE_2D, m_texture); // 텍스처 바인딩
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap);
}

void Texture::CreateTexture()
{
    glGenTextures(1, &m_texture);
    // bind and set default filter and wrap option
    Bind();
    SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}

void Texture::SetTextureFromImage(const Image *image)
{
    GLenum format = GL_RGBA; // 기본값으로 GL_RGBA 설정

    switch (image->GetChannelCount())
    {
    case 1:
        format = GL_RED;
        break;
    case 2:
        format = GL_RG;
        break;
    case 3:
        format = GL_RGB;
        break;
    case 4:
        format = GL_RGBA;
        break;
    default:
        break;
    }

    glBindTexture(GL_TEXTURE_2D, m_texture); // 텍스처 바인딩
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 image->GetWidth(), image->GetHeight(), 0,
                 format, GL_UNSIGNED_BYTE,
                 image->GetData());

    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        SPDLOG_ERROR("Failed to upload texture data, error code: {}", error);
    }

    glGenerateMipmap(GL_TEXTURE_2D);
}