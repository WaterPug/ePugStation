#ifndef E_PUG_STATION_VRAM
#define E_PUG_STATION_VRAM

#include "glad/glad.h"
#include <cstdint>
#include <array>

namespace ePugStation
{
   constexpr int VRAM_SIZE_4_bit = 1024 * 512 * 4;
   constexpr int VRAM_SIZE_16_bit = 1024 * 512;
    class VRAM
    {
    public:
        VRAM()
        {
            uint32_t buffer_mode = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;

            // 16bit VRAM pixel buffer
            //glGenBuffers(1, &m_pbo16);
            //glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo16);

            //glBufferStorage(GL_PIXEL_UNPACK_BUFFER, VRAM_SIZE_16_bit, nullptr, buffer_mode);
            //glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

            //glGenTextures(1, &m_texture16);
            //glBindTexture(GL_TEXTURE_2D, m_texture16);

            //// Set the texture wrapping parameters.
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            //// Set texture filtering parameters.
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            //// Allocate space on the GPU.
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, 1024, 512, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

            //glBindTexture(GL_TEXTURE_2D, m_texture16);
            //glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo16);

            //m_data16Bit = (uint16_t*)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, VRAM_SIZE_16_bit, buffer_mode);


            // 4bit VRAM pixel buffer.
            glGenBuffers(1, &m_pbo4);
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo4);

            glBufferStorage(GL_PIXEL_UNPACK_BUFFER, VRAM_SIZE_4_bit, nullptr, buffer_mode);
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

            glGenTextures(1, &m_texture4);
            glBindTexture(GL_TEXTURE_2D, m_texture4);

            // Set the texture wrapping parameters.
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Set texture filtering parameters.
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            // Allocate space on the GPU.
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 1024 * 4, 512, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

            glBindTexture(GL_TEXTURE_2D, m_texture4);
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo4);

            m_data4Bit = (uint8_t*)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, VRAM_SIZE_4_bit, buffer_mode);
        }

        uint16_t read(uint32_t x, uint32_t y)
        {
           int index = (y * 1024) + x;
           return m_data16Bit[index];
        }

        uint8_t read4(uint32_t x, uint32_t y) // x already in 4 bit pixel range
        {
           int index = (y * 4 * 1024) + x;
           return m_data4Bit[index];
        }

        void write(uint32_t x, uint32_t y, uint16_t data)
        {
            int index = (y * 1024) + x;

            // Write 16 bit data
            m_data16Bit[index] = data;

            // Write 4 bit data
            m_data4Bit[index * 4 + 0] = (uint8_t)data & 0xf;
            m_data4Bit[index * 4 + 1] = (uint8_t)(data >> 4) & 0xf;
            m_data4Bit[index * 4 + 2] = (uint8_t)(data >> 8) & 0xf;
            m_data4Bit[index * 4 + 3] = (uint8_t)(data >> 12) & 0xf;
        }

        // Call me after vram update...
        void uploadToGPU()
        {
            glBindTexture(GL_TEXTURE_2D, m_texture4);
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo4);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 4096, 512, GL_RED, GL_UNSIGNED_BYTE, 0);
        }

    private:
       // 16 bit related
       uint32_t m_pbo16;
       uint32_t m_texture16;
       uint16_t m_data16Bit[VRAM_SIZE_16_bit];

        // 4 bit related
        uint32_t m_pbo4;
        uint32_t m_texture4;
        uint8_t* m_data4Bit;
    };
}

#endif