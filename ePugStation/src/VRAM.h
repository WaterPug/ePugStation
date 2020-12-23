#ifndef E_PUG_STATION_VRAM
#define E_PUG_STATION_VRAM

#include "glad/glad.h"
#include <cstdint>
#include <array>

namespace ePugStation
{
    constexpr int VRAM_SIZE = 1024 * 512 * 4;
    class VRAM
    {
    public:
        VRAM()
        {
            uint32_t buffer_mode = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;

            // 16bit VRAM pixel buffer.
            glGenBuffers(1, &m_pbo);
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);

            glBufferStorage(GL_PIXEL_UNPACK_BUFFER, VRAM_SIZE, nullptr, buffer_mode);
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

            glGenTextures(1, &m_texture);
            glBindTexture(GL_TEXTURE_2D, m_texture);

            // Set the texture wrapping parameters.
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Set texture filtering parameters.
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            // Allocate space on the GPU.
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

            glBindTexture(GL_TEXTURE_2D, m_texture);
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);

            m_data = (uint8_t*)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, VRAM_SIZE, buffer_mode);
        }

        //uint16_t read(uint32_t x, uint32_t y);
        void write(uint32_t x, uint32_t y, uint16_t data)
        {
            int index = (y * 1024) + x;

            m_data[index * 4 + 0] = (uint8_t)data & 0xf;
            m_data[index * 4 + 1] = (uint8_t)(data >> 4) & 0xf;
            m_data[index * 4 + 2] = (uint8_t)(data >> 8) & 0xf;
            m_data[index * 4 + 3] = (uint8_t)(data >> 12) & 0xf;
        }

        // Call me after vram update...
        void uploadToGPU()
        {
            glBindTexture(GL_TEXTURE_2D, m_texture);
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1024 * 4, 512, GL_RED, GL_UNSIGNED_BYTE, m_data);
        }

    private:
        uint32_t m_pbo;
        uint32_t m_texture;
        uint8_t* m_data;
    };
}

#endif