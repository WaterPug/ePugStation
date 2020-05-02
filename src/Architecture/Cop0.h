#ifndef E_PUG_STATION_COP0
#define E_PUG_STATION_COP0

#include <cstdint>
#include "CPUExceptions.h"

namespace ePugStation
{
    struct Cop0_Cause {
        Cop0_Cause() = default;
        Cop0_Cause(uint32_t causeValue) : value(causeValue) {}

        union {
            uint32_t value;
            struct {
                unsigned unused1 : 2;
                unsigned excode : 5;
                unsigned unused2 : 1;
                unsigned ip : 8;
                unsigned unused3 : 12;
                unsigned CE : 2;
                unsigned unused4 : 1;
                unsigned BD : 1;
            } bit;
        };
    };

    struct Cop0_SR {
        Cop0_SR() = default;
        Cop0_SR(uint32_t causeValue) : value(causeValue) {}

        union {
            uint32_t value;
            struct {
                unsigned IEc : 1;
                unsigned KUc : 1;
                unsigned IEp : 1;
                unsigned KUp : 1;
                unsigned IEo : 1;
                unsigned KUo : 1;
                unsigned unused1 : 2;
                unsigned Im : 8;
                unsigned Isc : 1;
                unsigned Swc : 1;
                unsigned Pz : 1;
                unsigned CM : 1;
                unsigned PE : 1;
                unsigned TS : 1;
                unsigned BEV : 1;
                unsigned unused2 : 2;
                unsigned RE : 1;
                unsigned unused3 : 2;
                unsigned CU0 : 1;
                unsigned CU1 : 1;
                unsigned CU2 : 1;
                unsigned CU3 : 1;
            } bit;
        };
    };

    class Cop0
    {
    public:
        Cop0() : m_sr(0), m_cause(0), m_epc(0) {}
        ~Cop0() = default;

        uint32_t getSR() const;
        uint32_t getCause() const;
        uint32_t getEpc() const;

        void setSR(uint32_t value);
        void setCause(uint32_t value);
        void setEpc(uint32_t value);
        void setBranchDelayBit(uint32_t branchDelayBit);

        bool isCacheIsolated() const;
        bool isBootExceptionVectorsInROM() const;

        void updateOldInterupts();
        void updateNewInterupts();

        void updateExceptionCode(CPUException cpuException);

    private:
        // TODO: Fill with other registers ?
        Cop0_SR m_sr;       // Reg 12 : System status register (R/W)
        Cop0_Cause m_cause; // Reg 13 : Cause register
        uint32_t m_epc;     // Reg 14 : EPC
    };
}
#endif