#include "BIOS.h"
#include <fstream>
#include <intrin.h>

namespace ePugStation
{
	BIOS::BIOS()
	{
		m_data.fill(0);
		loadBIOS();
	}

	uint8_t BIOS::load8(uint32_t offset) const
	{
		return m_data[offset];
	}

	uint16_t BIOS::load16(uint32_t offset) const
	{
		auto b0 = static_cast<uint32_t>(m_data[offset]);
		auto b1 = static_cast<uint32_t>(m_data[offset + 1]);
		return	(b0 | (b1 << 8));
	}

	uint32_t BIOS::load32(uint32_t offset) const
	{
		auto b0 = static_cast<uint32_t>(m_data[offset]);
		auto b1 = static_cast<uint32_t>(m_data[offset + 1]);
		auto b2 = static_cast<uint32_t>(m_data[offset + 2]);
		auto b3 = static_cast<uint32_t>(m_data[offset + 3]);
		return	(b0 | (b1 << 8) | (b2 << 16) | (b3 << 24));
	}

	void BIOS::store8(uint32_t offset, uint8_t value)
	{
		m_data[offset] = value;
	}

	void BIOS::store16(uint32_t offset, uint16_t value)
	{
		m_data[offset] = static_cast<uint8_t>(value);
		m_data[offset + 1] = static_cast<uint8_t>(value >> 8);
	}

	void BIOS::store32(uint32_t offset, uint32_t value)
	{
		m_data[offset] = static_cast<uint8_t>(value);
		m_data[offset + 1] = static_cast<uint8_t>(value >> 8);
		m_data[offset + 2] = static_cast<uint8_t>(value >> 16);
		m_data[offset + 3] = static_cast<uint8_t>(value >> 24);
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

		if (!biosFile.read((char*)m_data.data(), length))
		{
			if (!biosFile.eof())
			{
				throw std::runtime_error("BIOS file size mismatch... Not at eof");
			}
		}
	}
}