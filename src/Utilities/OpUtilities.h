#ifndef E_PUG_STATION_OP_UTILITIES
#define E_PUG_STATION_OP_UTILITIES

#include <cstdint>

namespace ePugStation
{
    bool safeAdd(uint32_t a, uint32_t b, uint32_t& result);
    bool isAddOverflow(int32_t a, int32_t b, int32_t sum);
    bool isSubOverflow(int32_t a, int32_t b, int32_t sum);
}
#endif
