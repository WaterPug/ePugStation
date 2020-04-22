#ifndef E_PUG_STATION_CONSTANTS
#define E_PUG_STATION_CONSTANTS

#include <cstdint>
#include <string>
#include "Range.h"

namespace ePugStation
{
	// BIOS related
	constexpr uint32_t BIOS_ROM_LOGICAL = 0xbfc00000;
	constexpr uint32_t BIOS_ROM_PHYSICAL = 0x1fc00000;
	constexpr char PATH_TO_BIOS[] = "D:\\GitRepo\\ePugStation\\data\\SCPH1001.BIN";
	constexpr size_t BIOS_MEMORY_SIZE = 514 * 1024;
	constexpr Range<BIOS_ROM_PHYSICAL, BIOS_MEMORY_SIZE> BIOS_RANGE_PHYSICAL;

	// Mem control
	constexpr uint32_t MEM_CONTROL_START = 0x1f801000;
	constexpr uint32_t MEM_CONTROL_SIZE = 36;
	constexpr Range<MEM_CONTROL_START, MEM_CONTROL_SIZE> MEM_CONTROL_RANGE;

	// RAM SIZE
	constexpr uint32_t RAM_SIZE_START = 0x1f801060;
	constexpr uint32_t RAM_SIZE_SIZE = 4;
	constexpr Range<RAM_SIZE_START, RAM_SIZE_SIZE> RAM_SIZE_RANGE;

	// RAM
	constexpr uint32_t RAM_START_LOGICAL = 0xa0000000;
	constexpr uint32_t RAM_START_PHYSICAL = 0x00000000;
	constexpr uint32_t RAM_SIZE = 2 * 1024 * 1024;
	constexpr Range<RAM_START_PHYSICAL, RAM_SIZE> RAM_RANGE_PHYSICAL;

	// Cache_Control
	constexpr uint32_t CACHE_CONTROL_START = 0xfffe0130;
	constexpr uint32_t CACHE_CONTROL_SIZE = 4;
	constexpr Range<CACHE_CONTROL_START, CACHE_CONTROL_SIZE> CACHE_CONTROL_RANGE;

	// CPU related
	constexpr uint32_t CPU_REGISTERS = 32;

	// SPU
	constexpr uint32_t SPU_START = 0x1f801c00;
	constexpr uint32_t SPU_SIZE = 0xE80 - 0xC00;
	constexpr Range<SPU_START, SPU_SIZE> SPU_RANGE;

	// Expansion 1
	constexpr uint32_t EXPANSION_1_START = 0x1f000000;
	constexpr uint32_t EXPANSION_1_SIZE = 1024 * 1024; // TODO: Confirm this size
	constexpr Range<EXPANSION_1_START, EXPANSION_1_SIZE> EXPANSION_1_RANGE;

	// Expansion 2
	constexpr uint32_t EXPANSION_2_START = 0x1f802000;
	constexpr uint32_t EXPANSION_2_SIZE = 66;
	constexpr Range<EXPANSION_2_START, EXPANSION_2_SIZE> EXPANSION_2_RANGE;

	// Interrupt control
	constexpr uint32_t INTERRUPT_CONTROL_START = 0x1f801070;
	constexpr uint32_t INTERRUPT_CONTROL_SIZE = 8;
	constexpr Range<INTERRUPT_CONTROL_START, INTERRUPT_CONTROL_SIZE> INTERRUPT_CONTROL_RANGE;


	// General uses
	constexpr uint32_t ALIGNED_FOR_32_BITS = 4;
	constexpr uint32_t ALIGNED_FOR_16_BITS = 2;
}
#endif