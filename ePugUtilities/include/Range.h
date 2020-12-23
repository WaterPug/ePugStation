#ifndef E_PUG_STATION_RANGE
#define E_PUG_STATION_RANGE

#include <cstdint>

namespace ePugStation
{
    template<uint32_t START_ADDRESS, uint32_t LENGTH>
    class Range
    {
    public:
        Range() = default;
        ~Range() = default;

        bool contains(uint32_t address) const
        {
            return address >= START_ADDRESS && address < START_ADDRESS + LENGTH;
        }

        uint32_t offset(uint32_t address) const
        {
            return address - START_ADDRESS;
        }
    };
}
#endif