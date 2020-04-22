#include "Ram.h"

namespace ePugStation
{
	Ram::Ram()
	{
		m_data = std::make_unique<std::array<std::byte, RAM_SIZE>>();
		m_data->fill(std::byte(0xca));
	}

	uint8_t Ram::load8(uint32_t offset) const
	{
		return static_cast<uint8_t>(m_data->at(offset));
	}

	uint16_t Ram::load16(uint32_t offset) const
	{
		uint32_t b0 = static_cast<uint32_t>(m_data->at(offset));
		uint32_t b1 = static_cast<uint32_t>(m_data->at(offset + 1));
		return	b0 | (b1 << 8);
	}

	uint32_t Ram::load32(uint32_t offset) const
	{
		uint32_t b0 = static_cast<uint32_t>(m_data->at(offset));
		uint32_t b1 = static_cast<uint32_t>(m_data->at(offset + 1));
		uint32_t b2 = static_cast<uint32_t>(m_data->at(offset + 2));
		uint32_t b3 = static_cast<uint32_t>(m_data->at(offset + 3));
		return	b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
	}

	void Ram::store8(uint32_t offset, uint8_t value)
	{
		m_data->at(offset) = std::byte(value);
	}

	void Ram::store16(uint32_t offset, uint16_t value)
	{
		m_data->at(offset) = std::byte(value);
		m_data->at(offset + 1) = std::byte(value >> 8);
	}

	void Ram::store32(uint32_t offset, uint32_t value)
	{
		m_data->at(offset) = std::byte(value);
		m_data->at(offset + 1) = std::byte(value >> 8);
		m_data->at(offset + 2) = std::byte(value >> 16);
		m_data->at(offset + 3) = std::byte(value >> 24);
	}
}