#include <cstdint>
#include "OpUtilities.h"

namespace ePugStation
{
	bool safeAdd(uint32_t a, uint32_t b, uint32_t& result)
	{
		uint64_t t = a + b;
		result = static_cast<uint32_t>(t);
		return ((t & 0xFFFFFFFF00000000) == 0);
	}
}