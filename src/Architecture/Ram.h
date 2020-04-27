#ifndef E_PUG_STATION_RAM
#define E_PUG_STATION_RAM

#include "BIOS.h"
#include <memory>

namespace ePugStation
{
	class Ram
	{
	public:
		Ram();
		~Ram() = default;

		uint8_t load8(uint32_t offset) const;
		uint16_t load16(uint32_t offset) const;
		uint32_t load32(uint32_t offset) const;

		void store8(uint32_t offset, uint8_t value);
		void store16(uint32_t offset, uint16_t value);
		void store32(uint32_t offset, uint32_t value);

	private:
		std::array<uint8_t, RAM_SIZE> m_data;
	};
}
#endif