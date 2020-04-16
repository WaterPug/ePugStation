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

		uint32_t load32(uint32_t offset) const;
		void store32(uint32_t offset, uint32_t value);

	private:
		std::unique_ptr<std::array<std::byte, RAM_SIZE>> m_data; // unique_ptr to allocate on heap
	};
}
#endif