#ifndef E_PUG_STATION_CPU
#define E_PUG_STATION_CPU

#include "Utilities/Constants.h"
#include "Utilities/Instruction.h"
#include "Cop0.h"
#include "Interconnect.h"

#include <cstdint>
#include <unordered_map>
#include "CPUExceptions.h"

namespace ePugStation
{
    class CPU
    {
    public:
        CPU();
        ~CPU() = default;

        void runNextInstruction();
    private:
        Cop0 m_cop0;
        Instruction m_instruction;
        Interconnect m_interconnect;

        uint32_t m_ip;
        uint32_t m_currentIp;
        uint32_t m_nextIp;

        uint32_t m_HI;
        uint32_t m_LO;

        bool m_isBranching; // If branch occured in current instruction
        bool m_delaySlot;   // If last op was branch, we are in delay slot

        std::array<uint32_t, CPU_REGISTERS> m_registers;
        std::array<uint32_t, CPU_REGISTERS> m_outputRegisters;
        std::pair<uint32_t, uint32_t> m_loadPair;

        void decodeAndExecuteCurrentOp();

        void matchSubOp();
        void matchSubBranchOp();

        void setReg(uint32_t index, uint32_t value);
        void setReg(std::pair<uint32_t, uint32_t> setRegPair);

        // Loads
        uint8_t load8(uint32_t address) const;
        uint16_t load16(uint32_t address) const;
        uint32_t load32(uint32_t address) const;

        // Stores
        void store8(uint32_t address, uint8_t value);
        void store16(uint32_t address, uint16_t value);
        void store32(uint32_t address, uint32_t value);

        // Branch 
        void branch(uint32_t offset);

        // ADD
        void opADD();
        void opADDI();
        void opADDIU();
        void opADDU();

        // AND 
        void opAND();
        void opANDI();

        // Branch
        void opBEQ();
        void opBGEZ();
        void opBGEZAL();
        void opBGTZ();
        void opBLEZ();
        void opBLTZ();
        void opBLTZAL();
        void opBNE();

        // Division
        void opDIV();
        void opDIVU();

        // Jump
        void opJ();
        void opJAL();
        void opJR();
        void opJALR();

        // Loads
        void opLB();
        void opLBU();
        void opLH();
        void opLHU();
        void opLUI();
        void opLW();
        void opLWL();
        void opLWR();

        // Move
        void opMFHI();
        void opMFLO();
        void opMTHI();
        void opMTLO();

        // Mult
        void opMULT();
        void opMULTU();

        // Or
        void opOR();
        void opORI();

        // Store
        void opSB();
        void opSW();
        void opSH();
        void opSWL();
        void opSWR();

        // Shift
        void opSLL();
        void opSLLV();

        // Set
        void opSLT();
        void opSLTI();
        void opSLTIU();
        void opSLTU();

        // Shift right
        void opSRA();
        void opSRAV();
        void opSRL();
        void opSRLV();

        // SUB
        void opSUB();
        void opSUBU();

        // XOR
        void opXOR();
        void opXORI();
        void opNOR();

        // SYSCALL
        void opSYSCALL();
        void opBREAK();

        // Coprocessor operations
        void opCop0();
        void opCop1();
        void opCop2();
        void opCop3();
        void opMFC();
        void opMTC();
        void opRFE();
        void opLWC0();
        void opLWC1();
        void opLWC2();
        void opLWC3();
        void opSWC0();
        void opSWC1();
        void opSWC2();
        void opSWC3();

        // Illegal
        void opIllegal();

        void exception(CPUException exception);
    };
}
#endif