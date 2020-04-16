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

		uint32_t load32(uint32_t offset) const;
		void store32(uint32_t offset, uint32_t value);

	private:
		void loadBIOS();
		std::unique_ptr<std::array<std::byte, BIOS_MEMORY_SIZE>> m_data; // unique_ptr to allocate on heap
	};
}
#endif