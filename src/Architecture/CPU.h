#ifndef E_PUG_STATION_CPU
#define E_PUG_STATION_CPU

#include "Utilities/Constants.h"
#include "Interconnect.h"
#include "Utilities/Instruction.h"
#include <cstdint>

namespace ePugStation
{
	class CPU
	{
	public:
		CPU();
		~CPU() = default;

		uint8_t load8(uint32_t address) const;
		uint16_t load16(uint32_t address) const;
		uint32_t load32(uint32_t address) const;
		void store8(uint32_t address, uint8_t value);
		void store16(uint32_t address, uint16_t value);
		void store32(uint32_t address, uint32_t value);

		void runNextInstruction();
		void decodeAndExecute(uint32_t instruction);

	private:
		uint32_t m_ip;
		Instruction m_nextInstruction;
		std::array<uint32_t, CPU_REGISTERS> m_registers;
		std::array<uint32_t, CPU_REGISTERS> m_outputRegisters;
		std::pair<uint32_t, uint32_t> m_loadPair;
		Interconnect m_interconnect;

		uint32_t m_sr; // Temporary Cop0 register value. Sr = register #12 -- System status register (R/W)

		void matchOp(Instruction instruction);
		void matchSubOp(Instruction instruction);
		void matchSubBranchOp(Instruction instruction);

		void setReg(uint32_t index, uint32_t value);
		void setReg(std::pair<uint32_t, uint32_t> setRegPair);

		// Branch 
		void branch(uint32_t offset);

		// ADD
		void opADD(Instruction instruction);
		void opADDI(Instruction instruction);
		void opADDIU(Instruction instruction);
		void opADDU(Instruction instruction);

		// AND 
		void opAND(Instruction instruction);
		void opANDI(Instruction instruction);

		// Branch
		void opBEQ(Instruction instruction);
		void opBGEZ(Instruction instruction);
		void opBGEZAL(Instruction instruction);
		void opBGTZ(Instruction instruction);
		void opBLEZ(Instruction instruction);
		void opBLTZ(Instruction instruction);
		void opBLTZAL(Instruction instruction);
		void opBNE(Instruction instruction);

		// Division
		void opDIV(Instruction instruction);
		void opDIVU(Instruction instruction);

		// Jump
		void opJ(Instruction instruction);
		void opJAL(Instruction instruction);
		void opJR(Instruction instruction);

		// Loads
		void opLB(Instruction instruction);
		void opLH(Instruction instruction);
		void opLUI(Instruction instruction);
		void opLW(Instruction instruction);

		// Move
		void opMFHI(Instruction instruction);
		void opMFLO(Instruction instruction);

		// Mult
		void opMULT(Instruction instruction);
		void opMULTU(Instruction instruction);

		// Or
		void opOR(Instruction instruction);
		void opORI(Instruction instruction);

		// Store
		void opSB(Instruction instruction);
		void opSW(Instruction instruction);
		void opSH(Instruction instruction);

		// Shift
		void opSLL(Instruction instruction);
		void opSLLV(Instruction instruction);

		// Set
		void opSLT(Instruction instruction);
		void opSLTI(Instruction instruction);
		void opSLTIU(Instruction instruction);
		void opSLTU(Instruction instruction);

		// Shift right
		void opSRA(Instruction instruction);
		void opSRL(Instruction instruction);
		void opSRLV(Instruction instruction);

		// SUB
		void opSUB(Instruction instruction);
		void opSUBU(Instruction instruction);

		// XOR
		void opXOR(Instruction instruction);
		void opXORI(Instruction instruction);

		// SYSCALL
		void opSYSCALL(Instruction instruction);

		// Coprocessor operations
		void opCop0(Instruction instruction);
	};
}
#endif