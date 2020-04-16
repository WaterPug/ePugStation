#ifndef E_PUG_STATION_CONSTANTS
#define E_PUG_STATION_CONSTANTS

#include <cstdint>
#include <string>
#include "Range.h"

namespace ePugStation
{
	// BIOS related
	constexpr uint32_t BIOS_ROM = 0xbfc00000;
	constexpr char PATH_TO_BIOS[] = "D:\\GitRepo\\ePugStation\\ePugStation\\data\\SCPH1001.BIN";
	constexpr size_t BIOS_MEMORY_SIZE = 514 * 1024;
	constexpr Range<BIOS_ROM, BIOS_MEMORY_SIZE> BIOS_RANGE;

	// Mem control
	constexpr uint32_t EXPANSION_1_BASE = 0x1f000000;
	constexpr uint32_t EXPANSION_2_BASE = 0x1f802000;
	constexpr uint32_t MEM_CONTROL_START = 0x1f801000;
	constexpr uint32_t MEM_CONTROL_SIZE = 36;
	constexpr Range<MEM_CONTROL_START, MEM_CONTROL_SIZE> MEM_CONTROL_RANGE;

	// RAM SIZE
	constexpr uint32_t RAM_SIZE_START = 0x1f801060;
	constexpr uint32_t RAM_SIZE_SIZE = 4;
	constexpr Range<RAM_SIZE_START, RAM_SIZE_SIZE> RAM_SIZE_RANGE;

	// RAM
	constexpr uint32_t RAM_START = 0xa0000000;
	constexpr uint32_t RAM_SIZE = 2 * 1024 * 1024;
	constexpr Range<RAM_START, RAM_SIZE> RAM_RANGE;

	// Cache_Control
	constexpr uint32_t CACHE_CONTROL_START = 0xfffe0130;
	constexpr uint32_t CACHE_CONTROL_SIZE = 4;
	constexpr Range<CACHE_CONTROL_START, CACHE_CONTROL_SIZE> CACHE_CONTROL_RANGE;

	// CPU related
	constexpr uint32_t CPU_REGISTERS = 32;
}
#endif