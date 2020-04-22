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
	uint8_t Interconnect::load8(uint32_t address) const
	{
		uint32_t physicalAddress = maskRegion(address);

		if (BIOS_RANGE_PHYSICAL.contains(physicalAddress))
		{
			return m_bios.load8(BIOS_RANGE_PHYSICAL.offset(physicalAddress));
		}
		else if (EXPANSION_1_RANGE.contains(physicalAddress))
		{
			std::cout << "Unhandled EXPANSION 1 load8, returning 0xFF...\n";
			return 0xFF;
		}
		else if (EXPANSION_2_RANGE.contains(physicalAddress))
		{
			std::cout << "Unhandled EXPANSION 2 load8, returning 0xFF...\n";
			return 0xFF;
		}
		else if (RAM_RANGE_PHYSICAL.contains(physicalAddress))
		{
			return m_ram.load8(RAM_RANGE_PHYSICAL.offset(physicalAddress));
		}
		else
		{
			throw std::runtime_error("unhandled interconnect load8 address..." + std::to_string(physicalAddress));
		}
	}

	uint16_t Interconnect::load16(uint32_t address) const
	{
		checkIfAlignedBy<ALIGNED_FOR_16_BITS>(address);

		uint32_t physicalAddress = maskRegion(address);

		if (SPU_RANGE.contains(physicalAddress))
		{
			std::cout << "Unhandled SPU load16, ignoring...\n";
			throw std::runtime_error("unhandled interconnect load16 address..." + std::to_string(physicalAddress));
		}
		else
		{
			throw std::runtime_error("unhandled interconnect load16 address..." + std::to_string(physicalAddress));
		}
	}

	uint32_t Interconnect::load32(uint32_t address) const
	{
		checkIfAlignedBy<ALIGNED_FOR_32_BITS>(address);

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

	void Interconnect::store8(uint32_t address, uint8_t value)
	{
		uint32_t physicalAddress = maskRegion(address);

		if (EXPANSION_2_RANGE.contains(physicalAddress))
		{
			std::cout << "Unhandled EXPANSION 2 store8, ignoring...\n";
		}
		else if (RAM_RANGE_PHYSICAL.contains(physicalAddress))
		{
			m_ram.store8(RAM_RANGE_PHYSICAL.offset(physicalAddress), value);
		}
		else
		{
			throw std::runtime_error("unhandled interconnect stor16 address..." + std::to_string(physicalAddress));
		}
	}

	void Interconnect::store16(uint32_t address, uint16_t value)
	{
		checkIfAlignedBy<ALIGNED_FOR_16_BITS>(address);

		uint32_t physicalAddress = maskRegion(address);

		if (SPU_RANGE.contains(physicalAddress))
		{
			std::cout << "Unhandled SPU store16, ignoring...\n";
		}
		else
		{
			throw std::runtime_error("unhandled interconnect stor16 address..." + std::to_string(physicalAddress));
		}
	}

	void Interconnect::store32(uint32_t address, uint32_t value)
	{
		checkIfAlignedBy<ALIGNED_FOR_32_BITS>(address);

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
			if (offset == 0 && value != EXPANSION_1_START)
			{
				throw std::runtime_error("Expecting expansion 1 base address, got : " + std::to_string(physicalAddress));
			}
			else if (offset == 4 && value != EXPANSION_2_START)
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