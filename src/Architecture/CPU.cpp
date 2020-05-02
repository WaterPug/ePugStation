#include "CPU.h"
#include <Utilities/OpUtilities.h>
#include <Utilities/Utils.h>

#include <iostream>
#include <map>
#include <functional>

namespace ePugStation
{
    CPU::CPU() :
        m_cop0(),
        m_ip(BIOS_ROM_LOGICAL),
        m_HI(0xdeadbeaf),
        m_LO(0xdeadbeaf),
        m_isBranching(false),
        m_delaySlot(false),
        m_loadPair(std::make_pair(0, 0))
    {
        m_nextIp = m_ip + 4;

        m_registers.fill(0xdeadbeef);
        m_outputRegisters.fill(0xdeadbeef);
        m_registers[0] = 0; // Constant R0
    }

    void CPU::runNextInstruction()
    {
        m_instruction = Instruction(load32(m_ip));

        m_delaySlot = m_isBranching;
        m_isBranching = false;

        m_currentIp = m_ip;
        m_ip = m_nextIp;

        if (!checkIfAlignedBy<ALIGNED_FOR_32_BITS>(m_currentIp))
        {
            exception(CPUException::LoadAddressError);
            return;
        }

        // Point IP to next instruction
        m_nextIp += 4;
        setReg(m_loadPair);
        m_loadPair = std::make_pair(0, 0);
        decodeAndExecuteCurrentOp();
        m_registers = m_outputRegisters;
    }

    uint8_t CPU::load8(uint32_t address) const
    {
        return m_interconnect.load8(address);
    }

    void CPU::store8(uint32_t address, uint8_t value)
    {
        m_interconnect.store8(address, value);
    }

    uint16_t CPU::load16(uint32_t address) const
    {
        return m_interconnect.load16(address);
    }

    void CPU::store16(uint32_t address, uint16_t value)
    {
        m_interconnect.store16(address, value);
    }

    uint32_t CPU::load32(uint32_t address) const
    {
        return m_interconnect.load32(address);
    }

    void CPU::store32(uint32_t address, uint32_t value)
    {
        m_interconnect.store32(address, value);
    }

    void CPU::decodeAndExecuteCurrentOp()
    {
        auto it = m_primaryOpFunc.find(m_instruction.op.primary);
        if (it != m_primaryOpFunc.end())
        {
            it->second();
        }
        else
        {
            throw std::runtime_error("Unhandled primary operation");
        }
    }

    void CPU::matchSubBranchOp()
    {
        switch (m_instruction.reg.t)
        {
        case 0b00000: opBLTZ(); break;
        case 0b00001: opBGEZ(); break;
        case 0b10000: opBLTZAL(); break;
        case 0b10001: opBGEZAL(); break;
        default:
            throw std::runtime_error("Unsupported branch op with t value : " + std::to_string(m_instruction.reg.t));
        }
    }

    void CPU::matchSubOp()
    {
        auto it = m_secondaryOpFunc.find(m_instruction.op.seconday);
        if (it != m_secondaryOpFunc.end())
        {
            it->second();
        }
        else
        {
            throw std::runtime_error("Unhandled secondary operation");
        }
    }

    void CPU::setReg(uint32_t index, uint32_t value)
    {
        m_outputRegisters[index] = value;
        m_outputRegisters[0] = 0;
    }

    void CPU::setReg(std::pair<uint32_t, uint32_t> setRegPair)
    {
        setReg(setRegPair.first, setRegPair.second);
    }

    void CPU::branch(uint32_t offset)
    {
        m_nextIp += (offset << 2);
        m_nextIp -= 4;
        m_isBranching = true;
    }

    void CPU::opCop0()
    {
        switch (m_instruction.reg.s)
        {
        case 0b00000: opMFC(); break;
        case 0b00100: opMTC(); break;
        case 0b10000: opRFE(); break;
        default:
            throw std::runtime_error("Unhandled COP0 opcode...");
        }
    }

    void CPU::opCop1()
    {
        exception(CPUException::CoprocessorError); // Doesn't exist on Playstation
    }

    // GTE
    void CPU::opCop2()
    {
        throw std::runtime_error("Unhandled GTE instruction");
    }

    void CPU::opCop3()
    {
        exception(CPUException::CoprocessorError); // Doesn't exist on Playstation

    }

    // MFC : Move From Cop0
    void CPU::opMFC()
    {
        const uint32_t copIndex = m_instruction.reg.d;
        const uint32_t cpuIndex = m_instruction.reg.t;
        uint32_t loadValue = 0;

        switch (copIndex)
        {
        case 12:
            loadValue = m_cop0.getSR();
            break;
        case 13:
            loadValue = m_cop0.getCause();
            break;
        case 14:
            loadValue = m_cop0.getEpc();
            break;
        default:
            throw std::runtime_error("Unhandled Move from COP0 index value...");
        }

        // fill load
        m_loadPair = std::make_pair(cpuIndex, loadValue);
    }

    // Move to Cop0
    void CPU::opMTC()
    {
        auto regValue = m_registers[m_instruction.reg.t];

        // To be used when Cop0 registers are implemented
        // auto copRegIndex = m_instruction.reg.d;
        switch (m_instruction.reg.d)
        {
        case 3:
        case 5:
        case 6:
        case 7:
        case 9:
        case 11:
            if (regValue != 0)
                throw std::runtime_error("Unhandled write to cop0r");
            break;
        case 12:
            m_cop0.setSR(regValue);
            break;
        case 13:
            if (regValue != 0)
                m_cop0.setCause(regValue);
            break;
        case 14:
            if (regValue != 0)
                m_cop0.setEpc(regValue);
            break;
        default:
            throw std::runtime_error("Unhandled move to Cop0 index value...");
        }
    }

    void CPU::opLWC0()
    {
        exception(CPUException::CoprocessorError);
    }

    void CPU::opLWC1()
    {
        exception(CPUException::CoprocessorError);
    }

    void CPU::opLWC2()
    {
        throw std::runtime_error("Unhandled GTE LWC2 operation...");
    }

    void CPU::opLWC3()
    {
        exception(CPUException::CoprocessorError);
    }

    void CPU::opSWC0()
    {
        exception(CPUException::CoprocessorError);
    }

    void CPU::opSWC1()
    {
        exception(CPUException::CoprocessorError);
    }

    void CPU::opSWC2()
    {
        throw std::runtime_error("Unhandled GTE SWC2 operation...");
    }

    void CPU::opSWC3()
    {
        exception(CPUException::CoprocessorError);
    }

    void CPU::opIllegal()
    {
        std::cout << "Illegal instruction...\n";
        exception(CPUException::IllegalInstruction);
    }

    void CPU::opRFE()
    {
        if ((m_instruction.value & 0x3f) != 0b010000)
        {
            throw std::runtime_error("Invalid cop0 instrcution...");
        }

        // Previous moved to current and old to previous
        m_cop0.updateNewInterupts();
    }

    void CPU::opSLT()
    {
        auto s = static_cast<int32_t>(m_registers[m_instruction.reg.s]);
        auto t = static_cast<int32_t>(m_registers[m_instruction.reg.t]);

        setReg(m_instruction.reg.d, s < t);
    }

    void CPU::opSLTU()
    {
        setReg(m_instruction.reg.d, m_registers[m_instruction.reg.s] < m_registers[m_instruction.reg.t]);
    }

    void CPU::opSLTI()
    {
        auto s = static_cast<int32_t>(m_registers[m_instruction.reg.s]);

        setReg(m_instruction.reg.t, s < m_instruction.imm_se);
    }

    void CPU::opSLTIU()
    {
        setReg(m_instruction.reg.t, m_registers[m_instruction.reg.s] < m_instruction.imm);
    }

    void CPU::opSRA()
    {
        // For arithmetic shift : Signed integer
        int32_t signedValue = static_cast<int32_t>(m_registers[m_instruction.reg.t]);

        // Here need to keep signed bit
        setReg(m_instruction.reg.d, signedValue >> m_instruction.reg.h);
    }

    void CPU::opSRAV()
    {
        // For arithmetic shift : Signed integer
        int32_t signedValue = static_cast<int32_t>(m_registers[m_instruction.reg.t]);

        // Here need to keep signed bit
        setReg(m_instruction.reg.d, signedValue >> (m_registers[m_instruction.reg.s] & 0x1f));

    }

    void CPU::opSRL()
    {
        // For logical shift : Unsigned integer
        setReg(m_instruction.reg.d, m_registers[m_instruction.reg.t] >> m_instruction.reg.h);
    }

    void CPU::opSRLV()
    {
        setReg(m_instruction.reg.d, m_registers[m_instruction.reg.t] >> (m_registers[m_instruction.reg.s] & 0x1f));
    }

    void CPU::opSUB()
    {
        auto s = static_cast<int32_t>(m_registers[m_instruction.reg.s]);
        auto t = static_cast<int32_t>(m_registers[m_instruction.reg.t]);
        auto result = s - t;

        if (isSubOverflow(s, t, result))
        {
            exception(CPUException::Overflow);
        }
        else
        {
            setReg(m_instruction.reg.d, result);
        }
    }

    void CPU::opSUBU()
    {
        setReg(m_instruction.reg.d, m_registers[m_instruction.reg.s] - m_registers[m_instruction.reg.t]);
    }

    void CPU::opXOR()
    {
        setReg(m_instruction.reg.d, m_registers[m_instruction.reg.s] ^ m_registers[m_instruction.reg.t]);
    }

    void CPU::opXORI()
    {
        setReg(m_instruction.reg.t, m_registers[m_instruction.reg.s] ^ m_registers[m_instruction.imm]);
    }

    void CPU::opSYSCALL()
    {
        exception(CPUException::SysCall);
    }

    void CPU::opBREAK()
    {
        exception(CPUException::Break);
    }

    void CPU::opDIV()
    {
        int32_t numerator = static_cast<int32_t>(m_registers[m_instruction.reg.s]);
        int32_t denominator = static_cast<int32_t>(m_registers[m_instruction.reg.t]);

        if (denominator == 0) // Div by 0
        {
            m_HI = static_cast<uint32_t>(numerator);
            if (numerator >= 0)
            {
                m_LO = 0xFFFFFFFF;
            }
            else
            {
                m_LO = 0x1;
            }
        }
        else if (static_cast<uint32_t>(numerator) == 0x80000000 && denominator == -1)
        {
            m_LO = 0x80000000;
            m_HI = 0;
        }
        else
        {
            // Normal case
            m_LO = static_cast<uint32_t>(numerator / denominator);
            m_HI = static_cast<uint32_t>(numerator % denominator);
        }
    }

    void CPU::opDIVU()
    {
        uint32_t numerator = m_registers[m_instruction.reg.s];
        uint32_t denominator = m_registers[m_instruction.reg.t];

        if (denominator == 0) // Div by 0
        {
            m_HI = numerator;
            m_LO = 0xFFFFFFFF;
        }
        else
        {
            // Normal case
            m_LO = numerator / denominator;
            m_HI = numerator % denominator;
        }
    }

    void CPU::opOR()
    {
        setReg(m_instruction.reg.d, m_registers[m_instruction.reg.s] | m_registers[m_instruction.reg.t]);
    }

    void CPU::opORI()
    {
        setReg(m_instruction.reg.t, m_registers[m_instruction.reg.s] | m_instruction.imm);
    }

    void CPU::opNOR()
    {
        setReg(m_instruction.reg.d, !(m_registers[m_instruction.reg.s] | m_registers[m_instruction.reg.t]));
    }


    void CPU::opJ()
    {
        m_nextIp = (m_nextIp & 0xF0000000) | (m_instruction.immJump << 2);
        m_isBranching = true;
    }

    void CPU::opJAL()
    {
        setReg(31, m_nextIp);
        opJ();
    }

    void CPU::opJR()
    {
        m_nextIp = m_registers[m_instruction.reg.s];
        m_isBranching = true;
    }

    void CPU::opJALR()
    {
        setReg(m_instruction.reg.d, m_nextIp);
        m_nextIp = m_registers[m_instruction.reg.s];
        m_isBranching = true;
    }

    void CPU::opADDU()
    {
        setReg(m_instruction.reg.d, m_registers[m_instruction.reg.s] + m_registers[m_instruction.reg.t]);
    }

    void CPU::opAND()
    {
        setReg(m_instruction.reg.d, m_registers[m_instruction.reg.s] & m_registers[m_instruction.reg.t]);
    }

    void CPU::opANDI()
    {
        setReg(m_instruction.reg.t, m_registers[m_instruction.reg.s] & m_instruction.imm);
    }

    void CPU::opBNE()
    {
        if (m_registers[m_instruction.reg.s] != m_registers[m_instruction.reg.t])
        {
            branch(m_instruction.imm_se);
        }
    }

    void CPU::opBEQ()
    {
        if (m_registers[m_instruction.reg.s] == m_registers[m_instruction.reg.t])
        {
            branch(m_instruction.imm_se);
        }
    }

    void CPU::opBGEZ()
    {
        int32_t s = static_cast<int32_t>(m_registers[m_instruction.reg.s]);
        if (s >= 0)
        {
            branch(m_instruction.imm_se);
        }
    }

    void CPU::opBGEZAL()
    {
        int32_t s = static_cast<int32_t>(m_registers[m_instruction.reg.s]);
        if (s >= 0)
        {
            setReg(31, m_nextIp);
            branch(m_instruction.imm_se);
        }
    }

    void CPU::opBGTZ()
    {
        int32_t s = static_cast<int32_t>(m_registers[m_instruction.reg.s]);
        if (s > 0)
        {
            branch(m_instruction.imm_se);
        }
    }

    void CPU::opBLEZ()
    {
        int32_t s = static_cast<int32_t>(m_registers[m_instruction.reg.s]);
        if (s <= 0)
        {
            branch(m_instruction.imm_se);
        }
    }

    void CPU::opBLTZ()
    {
        int32_t s = static_cast<int32_t>(m_registers[m_instruction.reg.s]);
        if (s < 0)
        {
            branch(m_instruction.imm_se);
        }
    }

    void CPU::opBLTZAL()
    {
        int32_t s = static_cast<int32_t>(m_registers[m_instruction.reg.s]);
        if (s < 0)
        {
            setReg(31, m_nextIp);
            branch(m_instruction.imm_se);
        }
    }

    void CPU::opADDIU()
    {
        setReg(m_instruction.reg.t, m_registers[m_instruction.reg.s] + m_instruction.imm_se);
    }

    void CPU::opADD()
    {
        int32_t s = static_cast<int32_t>(m_registers[m_instruction.reg.s]);
        int32_t t = static_cast<int32_t>(m_registers[m_instruction.reg.t]);
        int32_t result = s + t;

        if (isAddOverflow(s, t, result))
        {
            exception(CPUException::Overflow);
        }
        else
        {
            setReg(m_instruction.reg.d, result);
        }
    }

    void CPU::opADDI()
    {
        int32_t s = static_cast<int32_t>(m_registers[m_instruction.reg.s]);
        int32_t result = s + m_instruction.imm_se;

        if (isAddOverflow(s, m_instruction.imm_se, result))
        {
            exception(CPUException::Overflow);
        }
        else
        {
            setReg(m_instruction.reg.t, result);
        }
    }

    void CPU::opSLL()
    {
        setReg(m_instruction.reg.d, m_registers[m_instruction.reg.t] << m_instruction.reg.h);
    }

    void CPU::opSLLV()
    {
        auto shiftCount = m_registers[m_instruction.reg.s] & 0x1f;
        setReg(m_instruction.reg.d, m_registers[m_instruction.reg.t] << shiftCount);
    }

    void CPU::opMFHI()
    {
        setReg(m_instruction.reg.d, m_HI);
    }

    void CPU::opMFLO()
    {
        setReg(m_instruction.reg.d, m_LO);
    }

    void CPU::opMTHI()
    {
        m_HI = m_registers[m_instruction.reg.s];
    }

    void CPU::opMTLO()
    {
        m_LO = m_registers[m_instruction.reg.s];
    }

    void CPU::opMULT()
    {
        int64_t s = static_cast<int32_t>(m_registers[m_instruction.reg.s]);
        int64_t t = static_cast<int32_t>(m_registers[m_instruction.reg.t]);
        uint64_t result = s * t;

        m_LO = static_cast<uint32_t>(result);
        m_HI = static_cast<uint32_t>(result >> 32);
    }

    void CPU::opMULTU()
    {
        uint64_t s = m_registers[m_instruction.reg.s];
        uint64_t t = m_registers[m_instruction.reg.t];
        uint64_t result = s * t;

        m_HI = static_cast<uint32_t>(result >> 32);
        m_LO = static_cast<uint32_t>(result);
    }

    void CPU::opSH()
    {
        if (m_cop0.isCacheIsolated())
        {
            return;
        }
        uint32_t address = m_registers[m_instruction.reg.s] + m_instruction.imm_se;
        if (checkIfAlignedBy<ALIGNED_FOR_16_BITS>(address))
        {
            store16(address, static_cast<uint16_t>(m_registers[m_instruction.reg.t]));
        }
        else
        {
            exception(CPUException::StoreAddressError);
        }
    }

    void CPU::opSB()
    {
        if (m_cop0.isCacheIsolated())
        {
            return;
        }
        uint32_t address = m_registers[m_instruction.reg.s] + m_instruction.imm_se;
        store8(address, 0xff & m_registers[m_instruction.reg.t]);
    }

    void CPU::opSW()
    {
        if (m_cop0.isCacheIsolated())
        {
            return;
        }
        uint32_t address = m_registers[m_instruction.reg.s] + m_instruction.imm_se;
        if (checkIfAlignedBy<ALIGNED_FOR_32_BITS>(address))
        {
            store32(address, m_registers[m_instruction.reg.t]);
        }
        else
        {
            exception(CPUException::StoreAddressError);
        }
    }

    void CPU::opLW()
    {
        if (m_cop0.isCacheIsolated())
        {
            return;
        }
        uint32_t address = m_registers[m_instruction.reg.s] + m_instruction.imm_se;
        if (checkIfAlignedBy<ALIGNED_FOR_32_BITS>(address))
        {
            uint32_t index = m_instruction.reg.t;
            m_loadPair = std::make_pair(index, load32(address));
        }
        else
        {
            exception(CPUException::LoadAddressError);
        }
    }

    // Store Word left
    void CPU::opSWL()
    {
        auto address = m_registers[m_instruction.reg.s] + m_instruction.imm_se;
        auto t = m_outputRegisters[m_instruction.reg.t];

        auto alignedAddress = address & !3;
        auto currentMem = load32(alignedAddress);

        uint32_t newMem = 0;
        switch (address & 3)
        {
        case 0: newMem = (currentMem & 0xffffff00) | (t >> 24); break;
        case 1: newMem = (currentMem & 0xffff0000) | (t >> 16); break;
        case 2: newMem = (currentMem & 0xff000000) | (t >> 8); break;
        case 3: newMem = (currentMem & 0x00000000) | t; break;
        }
        store32(address, newMem);
    }

    // Store Word right
    void CPU::opSWR()
    {
        auto address = m_registers[m_instruction.reg.s] + m_instruction.imm_se;
        auto t = m_outputRegisters[m_instruction.reg.t];

        auto alignedAddress = address & !3;
        auto currentMem = load32(alignedAddress);

        uint32_t newMem = 0;
        switch (address & 3)
        {
        case 0: newMem = (currentMem & 0x00000000) | t; break;
        case 1: newMem = (currentMem & 0x000000ff) | (t << 8); break;
        case 2: newMem = (currentMem & 0x0000ffff) | (t << 16); break;
        case 3: newMem = (currentMem & 0x00ffffff) | (t << 24); break;
        }
        store32(address, newMem);
    }

    // Load Word left
    void CPU::opLWL()
    {
        auto address = m_registers[m_instruction.reg.s] + m_instruction.imm_se;
        auto currentT = m_outputRegisters[m_instruction.reg.t];

        auto alignedAddress = address & !3;
        auto alignedWord = load32(alignedAddress);
        uint32_t result = 0;
        switch (address & 3)
        {
        case 0: result = (currentT & 0x00ffffff) | (alignedWord << 24); break;
        case 1: result = (currentT & 0x0000ffff) | (alignedWord << 16); break;
        case 2: result = (currentT & 0x000000ff) | (alignedWord << 8); break;
        case 3: result = (currentT & 0x00000000) | alignedWord; break;
        }
        uint32_t index = m_instruction.reg.t;
        m_loadPair = std::make_pair(index, result);
    }

    // Load Word right
    void CPU::opLWR()
    {
        auto address = m_registers[m_instruction.reg.s] + m_instruction.imm_se;

        auto curV = m_outputRegisters[m_instruction.reg.t];

        auto alignedAddress = address & !3;
        auto alignedWord = load32(alignedAddress);
        uint32_t result = 0;
        switch (address & 3)
        {
        case 0: result = (curV & 0x00000000) | alignedWord; break;
        case 1: result = (curV & 0xff000000) | (alignedWord >> 8); break;
        case 2: result = (curV & 0xffff0000) | (alignedWord >> 16); break;
        case 3: result = (curV & 0xffffff00) | (alignedWord >> 24); break;
        }
        uint32_t index = m_instruction.reg.t;
        m_loadPair = std::make_pair(index, result);
    }

    void CPU::opLB()
    {
        if (m_cop0.isCacheIsolated())
        {
            return;
        }
        uint32_t address = m_registers[m_instruction.reg.s] + m_instruction.imm_se;
        int8_t loadValue = load8(address);

        uint32_t index = m_instruction.reg.t;
        m_loadPair = std::make_pair(index, static_cast<uint32_t>(loadValue));
    }

    void CPU::opLBU()
    {
        if (m_cop0.isCacheIsolated())
        {
            return;
        }
        uint32_t address = m_registers[m_instruction.reg.s] + m_instruction.imm_se;

        uint32_t index = m_instruction.reg.t;
        m_loadPair = std::make_pair(index, load8(address));
    }

    void CPU::opLH()
    {
        if (m_cop0.isCacheIsolated())
        {
            return;
        }
        uint32_t address = m_registers[m_instruction.reg.s] + m_instruction.imm_se;
        if (checkIfAlignedBy<ALIGNED_FOR_16_BITS>(address))
        {
            int16_t loadValue = load16(address);
            uint32_t index = m_instruction.reg.t;
            m_loadPair = std::make_pair(index, static_cast<uint32_t>(loadValue));
        }
        else
        {
            exception(CPUException::LoadAddressError);
        }
    }

    void CPU::opLHU()
    {
        if (m_cop0.isCacheIsolated())
        {
            return;
        }
        uint32_t address = m_registers[m_instruction.reg.s] + m_instruction.imm_se;
        if (checkIfAlignedBy<ALIGNED_FOR_16_BITS>(address))
        {
            uint32_t index = m_instruction.reg.t;
            m_loadPair = std::make_pair(index, load16(address));
        }
        else
        {
            exception(CPUException::LoadAddressError);
        }
    }

    void CPU::opLUI()
    {
        setReg(m_instruction.reg.t, (m_instruction.imm << 16));
    }

    void CPU::exception(CPUException exception)
    {
        uint32_t handler = m_cop0.isBootExceptionVectorsInROM() ? 0xbfc00180 : 0x80000080;

        // Update Interrupt and Kernel/User mode bits
        m_cop0.updateOldInterupts();
        m_cop0.updateExceptionCode(exception);

        if (m_delaySlot)
        {
            m_cop0.setEpc(m_currentIp - 4);
            m_cop0.setBranchDelayBit(1);
        }
        else
        {
            m_cop0.setEpc(m_currentIp);
        }

        m_ip = handler;
        m_nextIp = m_ip + 4;
    }
}
