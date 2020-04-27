#ifndef E_PUG_STATION_BIOS
#define E_PUG_STATION_BIOS

#include "Utilities/Constants.h"
#include <array>
#include <cstddef>
#include <memory>

namespace ePugStation
{
	class BIOS
	{
	public:
		BIOS();
		~BIOS() = default;

		uint8_t load8(uint32_t offset) const;
		uint16_t load16(uint32_t offset) const;
		uint32_t load32(uint32_t offset) const;

		void store8(uint32_t offset, uint8_t value);
		void store16(uint32_t offset, uint16_t value);
		void store32(uint32_t offset, uint32_t value);

	private:
		void loadBIOS();
		std::array<uint8_t, BIOS_MEMORY_SIZE> m_data; // unique_ptr to allocate on heap
	};
}
#endif