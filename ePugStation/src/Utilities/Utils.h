#ifndef E_PUG_STATION_UTILS
#define E_PUG_STATION_UTILS

#include <cstdint>
#include <stdexcept>
#include <string>

namespace ePugStation
{
	void checkIf32BitAligned(uint32_t address)
	{
		if (address % 4 != 0)
		{
			throw std::runtime_error("address not aligned with address : " + std::to_string(address));
		}
	}
}
#endif