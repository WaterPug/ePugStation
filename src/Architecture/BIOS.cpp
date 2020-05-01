#include "BIOS.h"
#include <fstream>

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
		return static_cast<uint16_t>(((uint16_t*)(m_data.data()))[offset >> 1]);
	}

	uint32_t BIOS::load32(uint32_t offset) const
	{
		return static_cast<uint32_t>(((uint32_t*)(m_data.data()))[offset >> 2]);
	}

	void BIOS::store8(uint32_t offset, uint8_t value)
	{
		m_data[offset] = value;
	}

	void BIOS::store16(uint32_t offset, uint16_t value)
	{
		((uint16_t*)(m_data.data()))[offset >> 1] = value;
	}

	void BIOS::store32(uint32_t offset, uint32_t value)
	{
		((uint32_t*)(m_data.data()))[offset >> 2] = value;
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