#include "Ram.h"

namespace ePugStation
{
	Ram::Ram()
	{
		m_data.fill(0xca);
	}

	uint8_t Ram::load8(uint32_t offset) const
	{
		return m_data[offset];
	}

	uint16_t Ram::load16(uint32_t offset) const
	{
		uint32_t b0 = static_cast<uint32_t>(m_data[offset]);
		uint32_t b1 = static_cast<uint32_t>(m_data[offset + 1]);
		return	b0 | (b1 << 8);
	}

	uint32_t Ram::load32(uint32_t offset) const
	{
		auto b0 = static_cast<uint32_t>(m_data[offset]);
		auto b1 = static_cast<uint32_t>(m_data[offset + 1]);
		auto b2 = static_cast<uint32_t>(m_data[offset + 2]);
		auto b3 = static_cast<uint32_t>(m_data[offset + 3]);
		return	b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
	}

	void Ram::store8(uint32_t offset, uint8_t value)
	{
		m_data[offset] = value;
	}

	void Ram::store16(uint32_t offset, uint16_t value)
	{
		m_data[offset] = static_cast<uint8_t>(value);
		m_data[offset + 1] = static_cast<uint8_t>(value >> 8);
	}

	void Ram::store32(uint32_t offset, uint32_t value)
	{
		m_data[offset] = static_cast<uint8_t>(value);
		m_data[offset + 1] = static_cast<uint8_t>(value >> 8);
		m_data[offset + 2] = static_cast<uint8_t>(value >> 16);
		m_data[offset + 3] = static_cast<uint8_t>(value >> 24);
	}
}