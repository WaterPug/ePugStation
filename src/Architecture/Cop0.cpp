#include "Cop0.h"

namespace ePugStation
{
    uint32_t Cop0::getSR() const
    {
        return m_sr.value;
    }

    uint32_t Cop0::getCause() const
    {
        return m_cause.value;
    }

    uint32_t Cop0::getEpc() const
    {
        return m_epc;
    }

    void Cop0::setSR(uint32_t value)
    {
        m_sr.value = value;
    }

    void Cop0::setCause(uint32_t value)
    {
        m_cause.value = value;
    }

    void Cop0::setEpc(uint32_t value)
    {
        m_epc = value;
    }

    void Cop0::setBranchDelayBit(uint32_t branchDelayBit)
    {
        m_cause.bit.BD = branchDelayBit;
    }

    bool Cop0::isCacheIsolated() const
    {
        return m_sr.bit.Isc == 1;
    }

    bool Cop0::isBootExceptionVectorsInROM() const
    {
        return m_sr.bit.BEV == 1;
    }

    void Cop0::updateExceptionCode(CPUException cpuException)
    {
        m_cause.bit.excode = static_cast<uint8_t>(cpuException);
    }

    void Cop0::updateOldInterupts()
    {
        m_sr.bit.IEo = m_sr.bit.IEp;
        m_sr.bit.KUo = m_sr.bit.KUp;
        m_sr.bit.IEp = m_sr.bit.IEc;
        m_sr.bit.KUp = m_sr.bit.KUc;
    }

    void Cop0::updateNewInterupts()
    {
        m_sr.bit.IEc = m_sr.bit.IEp;
        m_sr.bit.KUc = m_sr.bit.KUp;
        m_sr.bit.IEp = m_sr.bit.IEo;
        m_sr.bit.KUp = m_sr.bit.KUo;
    }
}