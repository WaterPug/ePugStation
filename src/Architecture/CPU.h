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

        const std::unordered_map<PrimaryOp, std::function<void()>> m_primaryOpFunc{
            {PrimaryOp::SubOp, [this]() {return matchSubOp(); } },
            {PrimaryOp::BranchOp, [this]() {return matchSubBranchOp(); } },
            {PrimaryOp::opJ, [this]() {return opJ(); } },
            {PrimaryOp::opJAL, [this]() {return opJAL(); } },
            {PrimaryOp::opBEQ, [this]() {return opBEQ(); } },
            {PrimaryOp::opBNE, [this]() {return opBNE(); } },
            {PrimaryOp::opBLEZ, [this]() {return opBLEZ(); } },
            {PrimaryOp::opBGTZ, [this]() {return opBGTZ(); } },
            {PrimaryOp::opADDI, [this]() {return opADDI(); } },
            {PrimaryOp::opADDIU, [this]() {return opADDIU(); } },
            {PrimaryOp::opSLTI, [this]() {return opSLTI(); } },
            {PrimaryOp::opSLTIU, [this]() {return opSLTIU(); } },
            {PrimaryOp::opANDI, [this]() {return opANDI(); } },
            {PrimaryOp::opORI, [this]() {return opORI(); } },
            {PrimaryOp::opXORI, [this]() {return opXORI(); } },
            {PrimaryOp::opCop0, [this]() {return opCop0(); } },
            {PrimaryOp::opCop1, [this]() {return opCop1(); } },
            {PrimaryOp::opCop2, [this]() {return opCop2(); } },
            {PrimaryOp::opCop3, [this]() {return opCop3(); } },
            {PrimaryOp::opLUI, [this]() {return opLUI(); } },
            {PrimaryOp::opLB, [this]() {return opLB(); } },
            {PrimaryOp::opLBU, [this]() {return opLBU(); } },
            {PrimaryOp::opLH, [this]() {return opLH(); } },
            {PrimaryOp::opLWL, [this]() {return opLWL(); } },
            {PrimaryOp::opLW, [this]() {return opLW(); } },
            {PrimaryOp::opLHU, [this]() {return opLHU(); } },
            {PrimaryOp::opLWR, [this]() {return opLWR(); } },
            {PrimaryOp::opSB, [this]() {return opSB(); } },
            {PrimaryOp::opSH, [this]() {return opSH(); } },
            {PrimaryOp::opSWL, [this]() {return opSWL(); } },
            {PrimaryOp::opSW, [this]() {return opSW(); } },
            {PrimaryOp::opSWR, [this]() {return opSWR(); } },
            {PrimaryOp::opLWC0, [this]() {return opLWC0(); } },
            {PrimaryOp::opLWC1, [this]() {return opLWC1(); } },
            {PrimaryOp::opLWC2, [this]() {return opLWC2(); } },
            {PrimaryOp::opLWC3, [this]() {return opLWC3(); } },
            {PrimaryOp::opSWC0, [this]() {return opSWC0(); } },
            {PrimaryOp::opSWC1, [this]() {return opSWC1(); } },
            {PrimaryOp::opSWC2, [this]() {return opSWC2(); } },
            {PrimaryOp::opSWC3, [this]() {return opSWC3(); } }
        };

        const std::unordered_map<SecondaryOp, std::function<void()>> m_secondaryOpFunc{
            {SecondaryOp::opSLL, [this]() {return opSLL(); } },
            {SecondaryOp::opSRL, [this]() {return opSRL(); } },
            {SecondaryOp::opSRA, [this]() {return opSRA(); } },
            {SecondaryOp::opSLLV, [this]() {return opSLLV(); } },
            {SecondaryOp::opSRLV, [this]() {return opSRLV(); } },
            {SecondaryOp::opSRAV, [this]() {return opSRAV(); } },
            {SecondaryOp::opJR, [this]() {return opJR(); } },
            {SecondaryOp::opJALR, [this]() {return opJALR(); } },
            {SecondaryOp::opSYSCALL, [this]() {return opSYSCALL(); } },
            {SecondaryOp::opBREAK, [this]() {return opBREAK(); } },
            {SecondaryOp::opMFHI, [this]() {return opMFHI(); } },
            {SecondaryOp::opMTHI, [this]() {return opMTHI(); } },
            {SecondaryOp::opMFLO, [this]() {return opMFLO(); } },
            {SecondaryOp::opMTLO, [this]() {return opMTLO(); } },
            {SecondaryOp::opMULT, [this]() {return opMULT(); } },
            {SecondaryOp::opMULTU, [this]() {return opMULTU(); } },
            {SecondaryOp::opDIV, [this]() {return opDIV(); } },
            {SecondaryOp::opDIVU, [this]() {return opDIVU(); } },
            {SecondaryOp::opADD, [this]() {return opADD(); } },
            {SecondaryOp::opADDU, [this]() {return opADDU(); } },
            {SecondaryOp::opSUB, [this]() {return opSUB(); } },
            {SecondaryOp::opSUBU, [this]() {return opSUBU(); } },
            {SecondaryOp::opAND, [this]() {return opAND(); } },
            {SecondaryOp::opOR, [this]() {return opOR(); } },
            {SecondaryOp::opXOR, [this]() {return opXOR(); } },
            {SecondaryOp::opNOR, [this]() {return opNOR(); } },
            {SecondaryOp::opSLT, [this]() {return opSLT(); } },
            {SecondaryOp::opSLTU, [this]() {return opSLTU(); } }
        };

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