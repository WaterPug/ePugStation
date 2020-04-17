#include "Interconnect.h"
#include "Utilities/Constants.h"
#include "Utilities/Utils.h"

#include <iostream>

namespace
{
	constexpr std::array<uint32_t, 8> REGION_MASK = {
		// KUSEG: 2048MB
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		// KSEG0: 512MB
		0x7FFFFFFF,
		// KSEG1: 512MB
		0x1FFFFFFF,
		// KSEG2: 1024MB
		0xFFFFFFFF, 0xFFFFFFFF
	};

	uint32_t maskRegion(uint32_t address)
	{
		return address & REGION_MASK[address >> 29];
	}
}

namespace ePugStation
{
	uint32_t Interconnect::load8(uint32_t address) const
	{
		throw std::runtime_error("TODO");
	}

	uint32_t Interconnect::load16(uint32_t address) const
	{
		throw std::runtime_error("TODO");
	}

	uint32_t Interconnect::load32(uint32_t address) const
	{
		checkIf32BitAligned(address);

		uint32_t physicalAddress = maskRegion(address);

		if (BIOS_RANGE_PHYSICAL.contains(physicalAddress))
		{
			return m_bios.load32(BIOS_RANGE_PHYSICAL.offset(physicalAddress));
		}
		else if (RAM_RANGE_PHYSICAL.contains(physicalAddress))
		{
			return m_ram.load32(RAM_RANGE_PHYSICAL.offset(physicalAddress));
		}

		throw std::runtime_error("unhandled interconnect load address..." + std::to_string(physicalAddress));
	}

	void Interconnect::store8(uint32_t address, uint32_t value)
	{
		throw std::runtime_error("TODO");
	}

	void Interconnect::store16(uint32_t address, uint32_t value)
	{
		throw std::runtime_error("TODO");
	}

	void Interconnect::store32(uint32_t address, uint32_t value)
	{
		checkIf32BitAligned(address);

		uint32_t physicalAddress = maskRegion(address);

		if (BIOS_RANGE_PHYSICAL.contains(physicalAddress))
		{
			m_bios.store32(BIOS_RANGE_PHYSICAL.offset(physicalAddress), value);
		}
		else if (RAM_RANGE_PHYSICAL.contains(physicalAddress))
		{
			m_ram.store32(RAM_RANGE_PHYSICAL.offset(physicalAddress), value);
		}
		else if (MEM_CONTROL_RANGE.contains(physicalAddress))
		{
			uint32_t offset = MEM_CONTROL_RANGE.offset(physicalAddress);
			if (offset == 0 && value != EXPANSION_1_BASE)
			{
				throw std::runtime_error("Expecting expansion 1 base address, got : " + std::to_string(physicalAddress));
			}
			else if (offset == 4 && value != EXPANSION_2_BASE)
			{
				throw std::runtime_error("Expecting expansion 2 base address, got : " + std::to_string(physicalAddress));
			}
			else
			{
				std::cout << "Unhandled MEM_CONTROL store, ignoring...\n";
			}
		}
		else if (RAM_SIZE_RANGE.contains(physicalAddress))
		{
			std::cout << "Unhandled RAM_SIZE store, ignoring...\n";
		}
		else if (CACHE_CONTROL_RANGE.contains(physicalAddress))
		{
			std::cout << "Unhandled CACHE_CONTROL store, ignoring...\n";
		}
		else
		{
			throw std::runtime_error("Unhandled store..." + std::to_string(physicalAddress));
		}
	}
}