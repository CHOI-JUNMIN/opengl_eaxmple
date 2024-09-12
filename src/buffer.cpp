#include "buffer.h"

BufferUPtr Buffer::CreateWithData(uint32_t bufferType, uint32_t usage,
                                  const void *data, size_t stride, size_t count)
{
    auto buffer = BufferUPtr(new Buffer());
    if (!buffer->Init(bufferType, usage, data, stride, count))
        return nullptr;
    return std::move(buffer);
}

Buffer::~Buffer()
{
    if (m_buffer != 0)
    {
        glDeleteBuffers(1, &m_buffer);
        m_buffer = 0; 
    }
}

void Buffer::Bind() const
{
    static uint32_t lastBoundBuffer = 0;
    if (lastBoundBuffer != m_buffer)
    {
        glBindBuffer(m_bufferType, m_buffer);
        lastBoundBuffer = m_buffer;
    }
}

bool Buffer::Init(uint32_t bufferType, uint32_t usage, const void *data, size_t stride, size_t count)
{
    m_bufferType = bufferType;
    m_usage = usage;
    m_stride = stride;
    m_count = count;
    glGenBuffers(1, &m_buffer);
    Bind();

    // 변경되지 않는 데이터는 GL_STATIC_DRAW 사용
    glBufferData(m_bufferType, m_stride * m_count, data, GL_STATIC_DRAW); // 정적 데이터에 대해서는 GL_STATIC_DRAW 사용

    return true;
}