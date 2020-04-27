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
			return m_bios->load8(BIOS_RANGE_PHYSICAL.offset(physicalAddress));
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
			return m_ram->load8(RAM_RANGE_PHYSICAL.offset(physicalAddress));
		}
		else if (CDROM_RANGE.contains(physicalAddress))
		{
			std::cout << "Unhandled CDROM load 8, ignoring...\n";
			return 0;
		}
		else
		{
			throw std::runtime_error("unhandled interconnect load8 address..." + std::to_string(physicalAddress));
		}
	}

	uint16_t Interconnect::load16(uint32_t address) const
	{
		uint32_t physicalAddress = maskRegion(address);

		if (SPU_RANGE.contains(physicalAddress))
		{
			//std::cout << "Unhandled SPU load16, ignoring...\n";
			return 0;
		}
		else if (RAM_RANGE_PHYSICAL.contains(physicalAddress))
		{
			return m_ram->load16(RAM_RANGE_PHYSICAL.offset(physicalAddress));
		}
		else if (INTERRUPT_CONTROL_RANGE.contains(physicalAddress))
		{
			std::cout << "Unhandled INTERRUPT CONTROL load16, ignoring...\n";
			return 0;
		}
		else
		{
			throw std::runtime_error("unhandled interconnect load16 address..." + std::to_string(physicalAddress));
		}
	}

	uint32_t Interconnect::load32(uint32_t address) const
	{
		uint32_t physicalAddress = maskRegion(address);

		if (BIOS_RANGE_PHYSICAL.contains(physicalAddress))
		{
			return m_bios->load32(BIOS_RANGE_PHYSICAL.offset(physicalAddress));
		}
		else if (RAM_RANGE_PHYSICAL.contains(physicalAddress))
		{
			return m_ram->load32(RAM_RANGE_PHYSICAL.offset(physicalAddress));
		}
		else if (INTERRUPT_CONTROL_RANGE.contains(physicalAddress))
		{
			std::cout << "Unhandled INTERRUPT CONTROL load32, ignoring...\n";
			return 0;
		}
		else if (DMA_RANGE.contains(physicalAddress))
		{
			return getDMAReg(physicalAddress);
		}
		else if (GPU_RANGE.contains(physicalAddress))
		{
			auto offset = GPU_RANGE.offset(physicalAddress);
			return (offset == 4) ? 0x1c000000 : 0;
		}
		else if (TIMERS_RANGE.contains(physicalAddress))
		{
			std::cout << "Unhandled TIMERS load32, ignoring...\n";
			return 0;
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
			m_ram->store8(RAM_RANGE_PHYSICAL.offset(physicalAddress), value);
		}
		else if (CDROM_RANGE.contains(physicalAddress))
		{
			std::cout << "Unhandled CDROM store8, ignoring...\n";
		}
		else
		{
			throw std::runtime_error("unhandled interconnect stor16 address..." + std::to_string(physicalAddress));
		}
	}

	void Interconnect::store16(uint32_t address, uint16_t value)
	{
		uint32_t physicalAddress = maskRegion(address);

		if (SPU_RANGE.contains(physicalAddress))
		{
			//std::cout << "Unhandled SPU store16, ignoring...\n";
		}
		else if (TIMERS_RANGE.contains(physicalAddress))
		{
			std::cout << "Unhandled TIMERS store16, ignoring...\n";
		}
		else if (RAM_RANGE_PHYSICAL.contains(physicalAddress))
		{
			return m_ram->store16(RAM_RANGE_PHYSICAL.offset(physicalAddress), value);
		}
		else if (INTERRUPT_CONTROL_RANGE.contains(physicalAddress))
		{
			std::cout << "Unhandled INTERRUPT CONTROL store16, ignoring...\n";
		}
		else
		{
			throw std::runtime_error("unhandled interconnect stor16 address..." + std::to_string(physicalAddress));
		}
	}

	void Interconnect::store32(uint32_t address, uint32_t value)
	{
		uint32_t physicalAddress = maskRegion(address);

		if (BIOS_RANGE_PHYSICAL.contains(physicalAddress))
		{
			m_bios->store32(BIOS_RANGE_PHYSICAL.offset(physicalAddress), value);
		}
		else if (RAM_RANGE_PHYSICAL.contains(physicalAddress))
		{
			m_ram->store32(RAM_RANGE_PHYSICAL.offset(physicalAddress), value);
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
		else if (INTERRUPT_CONTROL_RANGE.contains(physicalAddress))
		{
			std::cout << "Unhandled INTERRUPT_CONTROL store, ignoring...\n";
		}
		else if (DMA_RANGE.contains(physicalAddress))
		{
			setDMAReg(physicalAddress, value);
		}
		else if (GPU_RANGE.contains(physicalAddress))
		{
			std::cout << "Unhandled GPU store, ignoring...\n";
		}
		else if (TIMERS_RANGE.contains(physicalAddress))
		{
			std::cout << "Unhandled TIMERS store, ignoring...\n";
		}
		else
		{
			throw std::runtime_error("Unhandled store..." + std::to_string(physicalAddress));
		}
	}

	uint32_t Interconnect::getDMAReg(uint32_t address) const
	{
		uint32_t offset = DMA_RANGE.offset(address);

		uint32_t major = (offset & 0x70) >> 4;
		uint32_t minor = offset & 0xf;

		if (major < 7)
		{
			if (minor == 0)
			{
				return m_dma.getChannelBaseAddress(major);
			}
			else if (minor == 4)
			{
				return m_dma.getChannelBlockControl(major);
			}
			if (minor == 8)
			{
				return m_dma.getChannelControl(major);
			}
		}
		else if (major == 7)
		{
			if (minor == 0)
			{
				return m_dma.getControl();
			}
			else if (minor == 4)
			{
				return m_dma.getInterrupt();
			}
		}
		throw std::runtime_error("Unhandled DMA access");
	}

	void Interconnect::executeDMATransfer(uint32_t index)
	{
		auto channel = m_dma.getChannel(index);

		if (channel.control.syncMode == SyncMode::LinkedList)
		{
			linkedListCopyDMA(index);
		}
		else
		{
			blockCopyDMA(index);
		}
	}

	void Interconnect::linkedListCopyDMA(uint32_t index)
	{
		auto channel = m_dma.getChannel(index);
		uint32_t address = channel.baseAddress & 0x1ffffc;
		if (channel.control.isFromRam == DMATransferDirection::ToRam) // to ram
		{
			throw std::runtime_error("Invalid direction for linked list mode");
		}
		// For now only GPU supported (to validate if linked list is used for other than the GPU
		if (index != 2)
		{
			throw std::runtime_error("Linked list only implemented for GPU");
		}

		while(true)
		{
			uint32_t header = m_ram->load32(address);
			uint32_t transferSize = header >> 24;
			while (transferSize > 0)
			{
				address = (address + 4) & 0x1ffffc;
				uint32_t command = m_ram->load32(address);
				std::cout << "GPU command" << command << "\n";
				--transferSize;
			}

			// Hardware seems to only check MSB (To validate)
			if ((header & 0x800000) != 0)
			{
				break;
			}
			address = header & 0x1ffffc;
		}
		m_dma.finalizeCopy(index);
	}

	void Interconnect::blockCopyDMA(uint32_t index)
	{
		auto channel = m_dma.getChannel(index);
		int32_t increment = channel.control.memoryAddressStep == StepDirection::Backward ? -4 : 4; // 1 == back, 0 == forward

		uint32_t address = channel.baseAddress;
		uint32_t transferSize = channel.getTransferSize();

		while (transferSize > 0)
		{
			uint32_t currentAddress = address & 0x1ffffc; // Masking hypothesis : RAM address wraps and two LSB are ignored
			uint32_t srcWord = 0;
			if (channel.control.isFromRam == DMATransferDirection::ToRam)
			{
				if (index == 6)
				{
					srcWord = transferSize == 1 ? 0xffffff : ((address - 4) & 0x1ffffc);
				}
				else
				{
					throw std::runtime_error("Unhandled DMA channel port");
				}
				m_ram->store32(currentAddress, srcWord);
			}
			else
			{
				srcWord = m_ram->load32(currentAddress);
				if (index == 2)
				{
					std::cout << "GPU command" << srcWord << "\n";
				}
				else
				{
					throw std::runtime_error("Unhandled DMA channel port");
				}
			}
			address += increment;
			--transferSize;
		}
		m_dma.finalizeCopy(index);
	}

	void Interconnect::setDMAReg(uint32_t address, uint32_t value)
	{
		uint32_t offset = DMA_RANGE.offset(address);

		uint32_t major = (offset & 0x70) >> 4;
		uint32_t minor = offset & 0xf;

		if (major < 7)
		{
			if (minor == 0)
			{
				m_dma.setChannelBaseAddress(major, value);
			}
			else if (minor == 4)
			{
				m_dma.setChannelBlockControl(major, value);
			}
			else if (minor == 8)
			{
				m_dma.setChannelControl(major, value);
			}

			if (m_dma.isChannelActive(major))
			{
				executeDMATransfer(major);
			}
			return;
		}
		else if (major == 7)
		{
			if (minor == 0)
			{
				m_dma.setControl(value);
				return;
			}
			else if (minor == 4)
			{
				m_dma.setInterrupt(value);
				return;
			}
		}
		throw std::runtime_error("Unhandled DMA access");
	}
}