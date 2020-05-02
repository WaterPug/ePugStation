#ifndef E_PUG_STATION_CPU_EXCEPTIONS
#define E_PUG_STATION_CPU_EXCEPTIONS

namespace ePugStation
{
    // TODO: Fill these with complete list
    enum class CPUException
    {
        LoadAddressError = 0x4,
        StoreAddressError = 0x5,
        SysCall = 0x8, // Syscall operation
        Break = 0x9, // Break operation
        IllegalInstruction = 0xa,
        CoprocessorError = 0xb,
        Overflow = 0xc // Arithmetic overflow
    };
}

#endif