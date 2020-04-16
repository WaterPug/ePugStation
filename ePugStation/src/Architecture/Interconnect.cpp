#include "Interconnect.h"
#include "Utilities/Constants.h"
#include "Utilities/Utils.h"

#include <iostream>

namespace ePugStation
{
	uint32_t Interconnect::load32(uint32_t address) const
	{
		checkIf32BitAligned(address);

		if (BIOS_RANGE.contains(address))
		{
			return m_bios.load32(BIOS_RANGE.offset(address));
		}
		else if (RAM_RANGE.contains(address))
		{
			return m_ram.load32(RAM_RANGE.offset(address));
		}

		throw std::runtime_error("BIOS address access out of range");
	}

	void Interconnect::store32(uint32_t address, uint32_t value)
	{
		checkIf32BitAligned(address);

		if (BIOS_RANGE.contains(address))
		{
			m_bios.store32(BIOS_RANGE.offset(address), value);
		}
		else if (RAM_RANGE.contains(address))
		{
			m_ram.store32(RAM_RANGE.offset(address), value);
		}
		else if (MEM_CONTROL_RANGE.contains(address))
		{
			uint32_t offset = MEM_CONTROL_RANGE.offset(address);
			if (offset == 0 && value != EXPANSION_1_BASE)
			{
				throw std::runtime_error("Expecting expansion 1 base address, got : " + std::to_string(address));
			}
			else if (offset == 4 && value != EXPANSION_2_BASE)
			{
				throw std::runtime_error("Expecting expansion 2 base address, got : " + std::to_string(address));
			}
			else
			{
				std::cout << "Unhandled MEM_CONTROL store, ignoring...\n";
			}
		}
		else if (RAM_SIZE_RANGE.contains(address))
		{
			std::cout << "Unhandled RAM_SIZE store, ignoring...\n";
		}
		else if (CACHE_CONTROL_RANGE.contains(address))
		{
			std::cout << "Unhandled CACHE_CONTROL store, ignoring...\n";
		}
		else
		{
			throw std::runtime_error("Unhandled store..." + std::to_string(address));
		}
	}
}