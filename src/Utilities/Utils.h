#ifndef E_PUG_STATION_UTILS
#define E_PUG_STATION_UTILS

#include <cstdint>
#include <stdexcept>
#include <string>

namespace ePugStation
{
	template<uint32_t BYTE_COUNT>
	void checkIfAlignedBy(uint32_t address)
	{
		if (address % BYTE_COUNT != 0)
		{
			throw std::runtime_error("address not aligned with address : " + std::to_string(address));
		}
	}
}
#endif