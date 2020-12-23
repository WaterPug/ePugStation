#ifndef E_PUG_STATION_INTERCONNECT
#define E_PUG_STATION_INTERCONNECT

#include "DMA.h"
#include "GPU.h"

#include <array>

namespace ePugStation
{
    class Interconnect
    {
    public:
        Interconnect() 
        { 
            loadBios();
            m_ram.fill(0xac);
        };
        ~Interconnect() = default;

        uint8_t load8(uint32_t address) const;
        uint16_t load16(uint32_t address) const;
        uint32_t load32(uint32_t address) const;
        void store8(uint32_t address, uint8_t value);
        void store16(uint32_t address, uint16_t value);
        void store32(uint32_t address, uint32_t value);

    private:
        uint32_t getDMAReg(uint32_t address) const;
        void setDMAReg(uint32_t address, uint32_t value);

        void executeDMATransfer(uint32_t index);
        void blockCopyDMA(uint32_t index);
        void linkedListCopyDMA(uint32_t index);

        void loadBios();

        std::array<uint8_t, BIOS_MEMORY_SIZE> m_bios;
        std::array<uint8_t, RAM_SIZE> m_ram;
        DMA m_dma;
        GPU m_gpu;
    };
}
#endif