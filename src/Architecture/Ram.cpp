#include "Ram.h"

namespace ePugStation
{
    Ram::Ram()
    {
        m_data.fill(0xca);
    }

    uint8_t Ram::load8(uint32_t offset) const
    {
        return m_data[offset];
    }

    // We assume that the offset is a multiple of 2 for load16 and multiple of 4 for load32
    // Worst case this will load closest aligned data...
    uint16_t Ram::load16(uint32_t offset) const
    {
        return static_cast<uint16_t>(((uint16_t*)(m_data.data()))[offset >> 1]);
    }

    uint32_t Ram::load32(uint32_t offset) const
    {
        return static_cast<uint32_t>(((uint32_t*)(m_data.data()))[offset >> 2]);
    }

    void Ram::store8(uint32_t offset, uint8_t value)
    {
        m_data[offset] = value;
    }

    void Ram::store16(uint32_t offset, uint16_t value)
    {
        ((uint16_t*)(m_data.data()))[offset >> 1] = value;
    }

    void Ram::store32(uint32_t offset, uint32_t value)
    {
        ((uint32_t*)(m_data.data()))[offset >> 2] = value;
    }
}