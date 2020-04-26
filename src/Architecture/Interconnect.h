#ifndef E_PUG_STATION_INTERCONNECT
#define E_PUG_STATION_INTERCONNECT

#include "BIOS.h"
#include "Ram.h"
#include "DMA.h"

namespace ePugStation
{
	class Interconnect
	{
	public:
		Interconnect() = default;
		~Interconnect() = default;

		uint8_t load8(uint32_t address) const;
		uint16_t load16(uint32_t address) const;
		uint32_t load32(uint32_t address) const;
		void store8(uint32_t address, uint8_t value);
		void store16(uint32_t address, uint16_t value);
		void store32(uint32_t address, uint32_t value);

	private:
		uint32_t getDMAReg(uint32_t address) const;
		void setDMAReg(uint32_t address, uint32_t value);

		void executeDMATransfer(uint32_t index);
		void blockCopyDMA(uint32_t index);
		void linkedListCopyDMA(uint32_t index);

		BIOS m_bios;
		Ram m_ram;
		DMA m_dma;
	};
}
#endif