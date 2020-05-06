#ifndef E_PUG_STATION_UTILS
#define E_PUG_STATION_UTILS

#include <cstdint>
#include <string>
#include <fstream>

namespace ePugStation
{
    template<uint32_t BYTE_COUNT>
    bool checkIfAlignedBy(uint32_t address)
    {
        return ((address % BYTE_COUNT) == 0);
    }

    inline std::string getFileContent(const std::string& path)
    {
        std::ifstream file(path);
        std::string content( (std::istreambuf_iterator<char>(file)),
                             (std::istreambuf_iterator<char>()    ));
        return content;
    }
}
#endif