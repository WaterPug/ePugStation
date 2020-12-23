#include "Interconnect.h"
#include "Constants.h"
#include "Utils.h"

#include <iostream>
#include <stdexcept>

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

    template<typename DATA_TYPE>
    constexpr int getDataShiftCount()
    {
        if constexpr (sizeof(DATA_TYPE) == 1)
        {
            return 0;
        }
        if constexpr (sizeof(DATA_TYPE) == 2)
        {
            return 1;
        }
        else if constexpr (sizeof(DATA_TYPE) == 4)
        {
            return 2;
        }
    }

    template<typename DATA_TYPE>
    void store(const uint8_t* data, uint32_t offset, DATA_TYPE value)
    {
        constexpr int shiftCount = getDataShiftCount<DATA_TYPE>();
        ((DATA_TYPE*)data)[offset >> shiftCount] = value;
    }

    template<typename DATA_TYPE>
        DATA_TYPE load(const uint8_t* data, uint32_t offset)
    {
        constexpr int shiftCount = getDataShiftCount<DATA_TYPE>();
        return static_cast<DATA_TYPE>(((DATA_TYPE*)data)[offset >> shiftCount]);
    }
}

namespace ePugStation
{
    uint8_t Interconnect::load8(uint32_t address) const
    {
        uint32_t physicalAddress = maskRegion(address);

        if (BIOS_RANGE_PHYSICAL.contains(physicalAddress))
        {
            uint32_t offset = BIOS_RANGE_PHYSICAL.offset(physicalAddress);
            return load<uint8_t>(m_bios.data(), offset);
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
            uint32_t offset = RAM_RANGE_PHYSICAL.offset(physicalAddress);
            return load<uint8_t>(m_ram.data(), offset);
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
            uint32_t offset = RAM_RANGE_PHYSICAL.offset(physicalAddress);
            return load<uint16_t>(m_ram.data(), offset);
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
            uint32_t offset = BIOS_RANGE_PHYSICAL.offset(physicalAddress);
            return load<uint32_t>(m_bios.data(), offset);
        }
        else if (RAM_RANGE_PHYSICAL.contains(physicalAddress))
        {
            uint32_t offset = RAM_RANGE_PHYSICAL.offset(physicalAddress);
            return load<uint32_t>(m_ram.data(), offset);
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
            if (offset == 0)
            {
                std::cout << "GPUREAD not implemented, ignoring...\n";
                return 0;
            }
            else
            {
                return m_gpu.getGPUStat().value;
            }
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
            uint32_t offset = RAM_RANGE_PHYSICAL.offset(physicalAddress);
            store<uint8_t>(m_ram.data(), offset, value);
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
            uint32_t offset = RAM_RANGE_PHYSICAL.offset(physicalAddress);
            store<uint16_t>(m_ram.data(), offset, value);
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
            uint32_t offset = BIOS_RANGE_PHYSICAL.offset(physicalAddress);
            store<uint32_t>(m_bios.data(), offset, value);
        }
        else if (RAM_RANGE_PHYSICAL.contains(physicalAddress))
        {
            uint32_t offset = RAM_RANGE_PHYSICAL.offset(physicalAddress);
            store<uint32_t>(m_ram.data(), offset, value);
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
            uint32_t offset = GPU_RANGE.offset(physicalAddress);
            if (offset == 0)
            {
                m_gpu.setGP0Command(value);
            }
            else
            {
                m_gpu.setGP1Command(value);
            }
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

        if (channel.control.bit.syncMode == SyncMode::LinkedList)
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
        if (channel.control.bit.isFromRam == DMATransferDirection::ToRam) // to ram
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
            uint32_t header = load<uint32_t>(m_ram.data(), address);
            uint32_t transferSize = header >> 24;
            while (transferSize > 0)
            {
                address = (address + 4) & 0x1ffffc;
                uint32_t command =  load<uint32_t>(m_ram.data(), address);
                m_gpu.setGP0Command(command);
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
        int32_t increment = channel.control.bit.memoryAddressStep == StepDirection::Backward ? -4 : 4; // 1 == back, 0 == forward

        uint32_t address = channel.baseAddress;
        uint32_t transferSize = channel.getTransferSize();

        while (transferSize > 0)
        {
            uint32_t currentAddress = address & 0x1ffffc; // Masking hypothesis : RAM address wraps and two LSB are ignored
            uint32_t srcWord = 0;
            if (channel.control.bit.isFromRam == DMATransferDirection::ToRam)
            {
                if (index == 6)
                {
                    srcWord = transferSize == 1 ? 0xffffff : ((address - 4) & 0x1ffffc);
                }
                else
                {
                    throw std::runtime_error("Unhandled DMA channel port");
                }
                store<uint32_t>(m_ram.data(), currentAddress, srcWord);
            }
            else
            {
                srcWord = load<uint32_t>(m_ram.data(), currentAddress);
                if (index == 2)
                {
                    m_gpu.setGP0Command(srcWord);
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

    void Interconnect::loadBios()
    {
        std::ifstream biosFile(PATH_TO_BIOS, std::ios::binary | std::ios::ate);
        if (!biosFile.is_open())
        {
            throw std::runtime_error("Failed to open BIOS file");
        }

        auto length = biosFile.tellg();
        biosFile.seekg(0, std::ios::beg);

        if (!biosFile.read((char*)m_bios.data(), length))
        {
            if (!biosFile.eof())
            {
                throw std::runtime_error("BIOS file size mismatch... Not at eof");
            }
        }
    }
}