#ifndef E_PUG_STATION_INTERCONNECT
#define E_PUG_STATION_INTERCONNECT

#include "BIOS.h"
#include "Ram.h"

namespace ePugStation
{
	class Interconnect
	{
	public:
		Interconnect() = default;
		~Interconnect() = default;

		uint32_t load8(uint32_t address) const;
		uint32_t load16(uint32_t address) const;
		uint32_t load32(uint32_t address) const;
		void store8(uint32_t address, uint32_t value);
		void store16(uint32_t address, uint32_t value);
		void store32(uint32_t address, uint32_t value);

	private:
		BIOS m_bios;
		Ram m_ram;
	};
}
#endif