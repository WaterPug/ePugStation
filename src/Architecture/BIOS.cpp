#include "BIOS.h"
#include <fstream>
#include <intrin.h>

namespace ePugStation
{
	BIOS::BIOS()
	{
		m_data = std::make_unique<std::array<std::byte, BIOS_MEMORY_SIZE>>();
		loadBIOS();
	}

	uint8_t BIOS::load8(uint32_t offset) const
	{
		return static_cast<uint8_t>(m_data->at(offset));
	}

	uint16_t BIOS::load16(uint32_t offset) const
	{
		uint32_t b0 = static_cast<uint32_t>(m_data->at(offset));
		uint32_t b1 = static_cast<uint32_t>(m_data->at(offset + 1));
		return	b0 | (b1 << 8);
	}

	uint32_t BIOS::load32(uint32_t offset) const
	{
		uint32_t b0 = static_cast<uint32_t>(m_data->at(offset));
		uint32_t b1 = static_cast<uint32_t>(m_data->at(offset + 1));
		uint32_t b2 = static_cast<uint32_t>(m_data->at(offset + 2));
		uint32_t b3 = static_cast<uint32_t>(m_data->at(offset + 3));
		return	b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
	}

	void BIOS::store8(uint32_t offset, uint8_t value)
	{
		m_data->at(offset) = std::byte(value);
	}

	void BIOS::store16(uint32_t offset, uint16_t value)
	{
		m_data->at(offset) = std::byte(value);
		m_data->at(offset + 1) = std::byte(value >> 8);
	}

	void BIOS::store32(uint32_t offset, uint32_t value)
	{
		m_data->at(offset) = std::byte(value);
		m_data->at(offset + 1) = std::byte(value >> 8);
		m_data->at(offset + 2) = std::byte(value >> 16);
		m_data->at(offset + 3) = std::byte(value >> 24);
	}

	void BIOS::loadBIOS()
	{
		std::ifstream biosFile(PATH_TO_BIOS, std::ios::binary | std::ios::ate);
		if (!biosFile.is_open())
		{
			throw std::runtime_error("Failed to open BIOS file");
		}

		auto length = biosFile.tellg();
		biosFile.seekg(0, std::ios::beg);

		if (!biosFile.read((char*)&m_data->at(0), length))
		{
			if (!biosFile.eof())
			{
				throw std::runtime_error("BIOS file size mismatch... Not at eof");
			}
		}
	}
}