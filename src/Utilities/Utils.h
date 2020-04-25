#ifndef E_PUG_STATION_UTILS
#define E_PUG_STATION_UTILS

#include <cstdint>
#include <stdexcept>
#include <string>

namespace ePugStation
{
	template<uint32_t BYTE_COUNT>
	bool checkIfAlignedBy(uint32_t address)
	{
		return ((address % BYTE_COUNT) == 0);
	}
}
#endif